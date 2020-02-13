/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "SshConnection.h"
#include "Debug.h"
#include "File.h"
#include "SshLoginDialog.h"
#include "SshSocket.h"
#include "SshTunnel.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTextStream>
#include <QTimer>

#include <numeric>
#include <fcntl.h>
#include <unistd.h>

#if WITH_SSH
#include <libssh/libssh.h>
#endif


namespace Ssh
{

    namespace
    {

        //_______________________________________________
        QString commandMessage( Connection::Command command )
        {
            using CommandHash = QHash<Connection::Command,QString>;
            static const CommandHash commandNames( {
                { Connection::Command::Connect, QObject::tr( "Connecting to host" ) },
                { Connection::Command::RequestIdentity, QObject::tr( "Waiting for user authentication" ) },
                { Connection::Command::AuthenticateWithGssAPI, QObject::tr( "Trying to authenticate using GssAPI" ) },
                { Connection::Command::AuthenticateWithAgent, QObject::tr( "Trying to authenticate using agent" ) },
                { Connection::Command::AuthenticateWithPassphrase, QObject::tr( "Trying to authenticate using passphrase" ) },
                { Connection::Command::AuthenticateWithPassword, QObject::tr( "Trying to authenticate using password" ) },
                { Connection::Command::KeepAlive, QObject::tr( "Keeping connection alive" ) }
            });

            return commandNames[command];
        }

        //_______________________________________________
        QString commandMessage( const Connection::CommandList& commands )
        {
            return std::accumulate( commands.begin(), commands.end(), QString(),
                []( QString in, Connection::Command command ) { return std::move( in ) + commandMessage( command ) + '\n'; } );
        }

    }

    //_______________________________________________
    Connection::Connection( QObject* parent ):
        QObject( parent ),
        Counter( QStringLiteral("Ssh::Connection") )
    {}

    //_______________________________________________
    Connection::~Connection()
    { disconnect(); }

    //_______________________________________________
    bool Connection::createTunnels()
    {

        Debug::Throw( QStringLiteral("Ssh::Connection::createTunnels.\n") );

        // loop over tunnel attributes
        for( const auto& attributes:connectionAttributes_.tunnels() )
        {

            // create Tcp server
            auto tcpServer = new QTcpServer( this );
            connect( tcpServer, &QTcpServer::newConnection, this, &Connection::_newConnection );
            if( !tcpServer->listen( QHostAddress::LocalHost, attributes.localPort() ) )
            {

                const auto message = tr( "Cannot listen to localhost:%1 - error:%2")
                    .arg( attributes.localPort() )
                    .arg( tcpServer->errorString() );

                Debug::Throw() << "Ssh::Connection::connectTunnels - " << message << endl;
                _notifyError( message );

                return false;
            }

        }

        // update state and return
        state_ |= TunnelCreated;

        return true;

    }

    //_______________________________________________
    bool Connection::connect( bool forceRequestIdentity )
    {

        Debug::Throw( QStringLiteral("Ssh::Connection::connect.\n") );

        #if WITH_SSH

        // session
        auto session = ssh_new();
        if( !session )
        {
            Debug::Throw() << "Ssh::Connection::connect - Cannot initialize session" << endl;
            _notifyError( tr( "cannot initialize ssh session" ) );
            return false;
        }

        // cast session
        session_.reset( session );

        // update state and return
        state_ |= SessionCreated;

        // mark as non blocking
        ssh_set_blocking(session, false);

        // request commands
        /* must request identity first because some authentication
        requires the user name to be set before connecting */
        CommandList commands;
        if( forceRequestIdentity ) commands.append(Command::RequestIdentity);
        commands.append(Command::Connect);

        // request command
        addCommands( commands );

        return true;
        #else
        return false;
        #endif

    }

    //_______________________________________________
    bool Connection::authenticate( bool forceRequestIdentity )
    {

        Debug::Throw() << "Ssh::Connection::authenticate - forceRequestIdentity: " << forceRequestIdentity << endl;

        CommandList commands( {
            Command::AuthenticateWithGssAPI,
            Command::AuthenticateWithAgent } );

        // ask for password if needed and not already done
        if( !( forceRequestIdentity || connectionAttributes_.rememberPassword() ) )
        { commands.append(Command::RequestIdentity); }

        commands.append(Command::AuthenticateWithPassphrase);
        commands.append(Command::AuthenticateWithPassword);

        addCommands( commands );
        return true;

    }

    //_______________________________________________________________________
    bool Connection::keepAlive()
    {
        if( !commands_.contains( Command::KeepAlive ) )
        {

            addCommand( Command::KeepAlive );
            return true;

        } else return false;
    }


    //_______________________________________________________________________
    bool Connection::waitForConnected( int msecs )
    {

        // do nothing if already connected
        if( isConnected() ) return true;

        /*
        indicate that timer should not be restarted
        and commands not processed from timerEvent
        this prevents some race conditions
        */
        timerLocked_ = true;

        // stop timer
        if( timer_.isActive() ) timer_.stop();

        // start timer
        QElapsedTimer timer;
        timer.start();

        while( (msecs < 0 || timer.elapsed() < msecs) && _processCommands() )
        { qApp->processEvents(); }

        // reset flag
        timerLocked_ = false;

        return isConnected();

    }

    //_______________________________________________
    void Connection::addCommands( const CommandList& commands )
    {
        Debug::Throw() << "Ssh::Connection::AddCommand: " << commandMessage( commands ) << endl;
        commands_.append( commands );
        if( !( timer_.isActive() || timerLocked_ ) )
        { timer_.start( latency_, this ); }
    }

    //_______________________________________________
    void Connection::addCommand( Command command )
    {
        Debug::Throw() << "Ssh::Connection::AddCommand: " << commandMessage( command ) << endl;
        commands_.append( command );
        if( !( timer_.isActive() || timerLocked_ ) )
        { timer_.start( latency_, this ); }

    }

    //_______________________________________________
    void Connection::abortCommands()
    {
        if( timer_.isActive() ) timer_.stop();
        commands_.clear();
    }

    //_______________________________________________
    void Connection::disconnect()
    {

        Debug::Throw( QStringLiteral("Ssh::Connection::disconnect.\n") );
        _disconnectChannels();
        _disconnectTunnels();
        _disconnectSession();
        emit disconnected();

    }

    //_______________________________________________
    void Connection::_disconnectChannels()
    {

        Debug::Throw( QStringLiteral("Ssh::Connection::_disconnectChannels.\n") );

        // loop over tunnels and delete
        for( const auto& tunnel:findChildren<Tunnel*>() )
        {
            tunnel->close();
            tunnel->deleteLater();
        }

    }

    //_______________________________________________
    void Connection::_disconnectSession()
    {

        Debug::Throw( QStringLiteral("Ssh::Connection::_disconnectSession.\n") );
        session_.reset();
        state_ &= ~(Connected|SessionCreated);
    }

    //_______________________________________________
    void Connection::_disconnectTunnels()
    {
        Debug::Throw( QStringLiteral("Ssh::Connection::_disconnectTunnel.\n") );

        // disconnect all tcp servers
        for( const auto& tcpServer:findChildren<QTcpServer*>() )
        {
            if( tcpServer->isListening() ) tcpServer->close();
            tcpServer->deleteLater();
        }

        // update state
        state_ &= ~TunnelCreated;

    }

    //_______________________________________________
    bool Connection::isSupported()
    {
        #if WITH_SSH
        return true;
        #else
        return false;
        #endif
    }

    //_______________________________________________
    void Connection::timerEvent( QTimerEvent* event )
    {
        if( event->timerId() == timer_.timerId() )
        {
            if( ( timerLocked_ || !_processCommands() ) && timer_.isActive() )
            { timer_.stop(); }
        } QObject::timerEvent( event );
    }

    //_______________________________________________
    bool Connection::_processCommands()
    {

        // check empty commands list
        if( commands_.empty() ) return false;

        // check session
        if( !session_ )
        {
            _abortCommands( tr( "Session is invalid" ) );
            return false;
        }

        // emit message
        if( commands_.front() != lastCommand_ )
        {
            lastCommand_ = commands_.front();
            _notifyMessage( commandMessage( lastCommand_ ) );
        }

        #if WITH_SSH

        Debug::Throw() << "Ssh::Connection::_processCommands -"
            << " host: " << connectionAttributes_.host()
            << " processing command: " << commandMessage(commands_.front())
            << endl;

        // cast session. It is used for almost all commands
        auto session( static_cast<ssh_session>(session_.get()) );
        switch( commands_.front() )
        {

            case Command::Connect:
            {

                auto port( connectionAttributes_.port() );

                if( !connectionAttributes_.host().isEmpty() )
                { ssh_options_set( session, SSH_OPTIONS_HOST, qPrintable( connectionAttributes_.host() ) ); }

                if( !connectionAttributes_.user().isEmpty() )
                { ssh_options_set( session, SSH_OPTIONS_USER, qPrintable( connectionAttributes_.user() ) ); }

                ssh_options_set( session, SSH_OPTIONS_PORT, &port );

                Debug::Throw() << "Ssh::Connection::_processCommands - connection."
                    << " Host: " << connectionAttributes_.host()
                    << " Port: " << connectionAttributes_.port()
                    << " User: " << connectionAttributes_.user()
                    << endl;

                auto result = ssh_connect(session);
                if( result == SSH_OK )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( result == SSH_ERROR ) {

                    _abortCommands( tr( "Cannot connect to host %1:%2 - %3" )
                        .arg( connectionAttributes_.host() )
                        .arg( connectionAttributes_.port() )
                        .arg( ssh_get_error( session ) ) );

                    return false;

                }

            }
            break;

            case Command::RequestIdentity:
            {

                // login dialog
                LoginDialog dialog( nullptr );
                dialog.setAttributes( connectionAttributes_ );
                if( dialog.exec() )
                {

                    // get updated attributes
                    const auto attributes( dialog.attributes() );

                    // detect changes, save, and emit signal if needed
                    if( ( connectionAttributes_.user() != attributes.user() ) ||
                        ( connectionAttributes_.password() != attributes.password() ) ||
                        ( connectionAttributes_.rememberPassword() != attributes.rememberPassword() ) )
                    {
                        connectionAttributes_ = attributes;
                        emit attributesChanged();
                    }

                    commands_.removeFirst();
                    return true;

                } else {

                    _abortCommands( tr( "Login canceled" ) );
                    return false;

                }

            }
            break;

            case Command::AuthenticateWithGssAPI:
            {

                auto result = ssh_userauth_gssapi( session );
                if( result == SSH_AUTH_SUCCESS )
                {

                    Debug::Throw( QStringLiteral("Ssh::Connection::_processCommands - connected.\n") );

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();
                    return false;

                } else if( result != SSH_AUTH_AGAIN ) {

                    // ignore error, move to next command
                    commands_.removeFirst();
                    return true;

                }

                break;
            }

            case Command::AuthenticateWithAgent:
            {

                #if !defined(Q_OS_WIN)
                auto result = ssh_userauth_agent( session, qPrintable( connectionAttributes_.user() ) );
                if( result == SSH_AUTH_SUCCESS )
                {

                    Debug::Throw( QStringLiteral("Ssh::Connection::_processCommands - connected.\n") );

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();
                    return false;

                } else if( result != SSH_AUTH_AGAIN ) {

                    // ignore error, move to next command
                    commands_.removeFirst();
                    return true;

                }
                #else

                // ignore error, move to next command
                commands_.removeFirst();
                return true;

                #endif
                break;
            }

            case Command::AuthenticateWithPassphrase:
            {

                Debug::Throw() << "Ssh::Connection::_processCommands - passphrase authentication."
                    << " Host: " << connectionAttributes_.host()
                    << " User: " << connectionAttributes_.user()
                    << " Password: " << connectionAttributes_.password()
                    << endl;

                auto result = ssh_userauth_publickey_auto( session, nullptr, qPrintable( connectionAttributes_.password() ) );
                if( result == SSH_AUTH_SUCCESS )
                {

                    Debug::Throw( QStringLiteral("Ssh::Connection::_processCommands - connected.\n") );

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();
                    return false;

               } else if( result != SSH_AUTH_AGAIN ) {

                    // ignore error, move to next command
                    commands_.removeFirst();
                    return true;

                }

                break;
            }

            case Command::AuthenticateWithPassword:
            {

                Debug::Throw() << "Ssh::Connection::_processCommands - password authentication."
                    << " Host: " << connectionAttributes_.host()
                    << " User: " << connectionAttributes_.user()
                    << " Password: " << connectionAttributes_.password()
                    << endl;

                auto result = ssh_userauth_password( session, qPrintable( connectionAttributes_.user() ), qPrintable( connectionAttributes_.password() ) );
                if( result == SSH_AUTH_SUCCESS )
                {

                    Debug::Throw( QStringLiteral("Ssh::Connection::_processCommands - connected.\n") );

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();
                    return false;

                } else if( result != SSH_AUTH_AGAIN ) {

                    commands_.clear();
                    emit loginFailed();

                    // start over
                    _disconnectSession();
                    connect( true );
                    authenticate( true );
                    return true;

                }

                break;
            }

            case Command::KeepAlive:
            {
                static constexpr const char* aliveData = "";
                auto result = ssh_send_ignore( session, aliveData );
                if( result == SSH_OK )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( result == SSH_ERROR ) {

                    // failure
                    commands_.removeFirst();
                    return false;

                }

                break;

            }
            default:
            commands_.removeFirst();
            return true;

        }

        return true;


        #else

        _abortCommands( tr( "No SSH support" ) );
        return false;

        #endif

    }

    //_______________________________________________
    void Connection::_notifyError( const QString &error )
    {
        Debug::Throw(0) << "Ssh::Connection::_notifyError: " << error << endl;
        error_ = error;
        emit this->error( error );
        emit disconnected();
    }

    //_______________________________________________
    void Connection::_notifyMessage( const QString &message )
    {
        Debug::Throw() << "Ssh::Connection::_notifyMessage: " << message << endl;
        emit this->message( message );
    }

    //_______________________________________________
    void Connection::_notifyDebug( const QString &message )
    { Debug::Throw() << message << endl; }

    //_______________________________________________
    void Connection::_newConnection()
    {
        Debug::Throw( QStringLiteral("Ssh::Connection::_newConnection.\n") );

        // check session
        if( !session_ )
        {
            Debug::Throw( QStringLiteral("Ssh::Connection::_newConnection - invalid session.\n") );
            return;
        }

        // loop over tcp servers
        for( const auto& tcpServer:findChildren<QTcpServer*>() )
        {

            // check pending connection
            if( !tcpServer->hasPendingConnections() ) continue;

            // find tunnel attributes matching host
            TunnelAttributes attributes;
            attributes.setLocalPort( tcpServer->serverPort() );

            const auto tunnels( connectionAttributes_.tunnels() );
            const auto iter( tunnels.constFind( attributes ) );
            if( iter == tunnels.cend() )
            {
                Debug::Throw(0) << "Ssh::Connection::_newConnection - unable to find tunnel attributes matching port " << tcpServer->serverPort() << endl;
                continue;
            }

            while( auto tcpSocket = tcpServer->nextPendingConnection() )
            {
                auto tunnel = new Tunnel( this, tcpSocket );
                tunnel->sshSocket()->setLatency( tunnelLatency_);
                tunnel->sshSocket()->setLocalHost( QStringLiteral("localhost"), iter->localPort() );
                tunnel->sshSocket()->connectToHost( session_.get(), iter->host(), iter->remotePort() );

                connect( tunnel, &Tunnel::error, this, &Connection::_notifyError );
                connect( tunnel, &Tunnel::debug, this, &Connection::_notifyDebug );
                connect( tunnel->sshSocket(), &Socket::error, tunnel, &QObject::deleteLater );
                connect( tunnel->sshSocket(), &QIODevice::readChannelFinished, tunnel, &QObject::deleteLater );
            }

        }

    }

    //_______________________________________________
    void Connection::_abortCommands( const QString& errorMessage )
    {
        Debug::Throw( QStringLiteral("Ssh::Connection::_abortCommands.\n") );
        if( timer_.isActive() ) timer_.stop();
        commands_.clear();
        _notifyError( errorMessage );
    }

}

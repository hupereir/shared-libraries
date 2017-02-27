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
#include "Sleep.h"
#include "SshLoginDialog.h"
#include "SshSingleton.h"
#include "SshSocket.h"
#include "SshTunnel.h"

#include <QElapsedTimer>
#include <QTcpServer>
#include <QTextStream>
#include <QTimer>
#include <QThread>
#if defined(Q_OS_WIN)
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

#include <unistd.h>
#include <fcntl.h>

#if HAVE_SSH
#include <libssh2.h>
#endif


namespace Ssh
{

    //_______________________________________________
    Connection::Connection( QObject* parent ):
        QObject( parent ),
        Counter( "Ssh::Connection" )
    {}

    //_______________________________________________
    Connection::~Connection( void )
    { disconnect(); }

    //_______________________________________________
    bool Connection::createTunnels( void )
    {

        Debug::Throw( "Ssh::Connection::createTunnels.\n" );
        if( !Singleton::get().initialized() ) return false;

        // loop over tunnel attributes
        for( const auto& attributes:attributes_.tunnels() )
        {

            // create Tcp server
            QTcpServer* tcpServer = new QTcpServer( this );
            connect( tcpServer, SIGNAL(newConnection()), this, SLOT(_newConnection()) );
            if( !tcpServer->listen( QHostAddress::LocalHost, attributes.localPort() ) )
            {

                const QString message = tr( "Cannot listen to localhost:%1 - error:%2")
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
    bool Connection::connect( void )
    {

        Debug::Throw( "Ssh::Connection::connect.\n" );
        if( !Singleton::get().initialized() ) return false;

        #if HAVE_SSH
        // initialize socket
        sshSocket_ = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if( sshSocket_ == -1 )
        {
            _notifyError( tr( "unable to create ssh socket" ) );
            return false;
        }

        // make socket non blocking
        const int flags( fcntl(sshSocket_, F_GETFL ) );
        if( flags < 0 )
        {
            _notifyError( tr( "unable to get socket flags" ) );
            return false;
        }

        if( fcntl( sshSocket_, F_SETFL, flags|O_NONBLOCK ) < 0 )
        {
            _notifyError( tr( "unable to set socket flags" ) );
            return false;
        }

        // lookup host
        QHostInfo::lookupHost( attributes_.host(), this, SLOT(_saveHost(QHostInfo)) );

        // session
        if(!( session_ = libssh2_session_init() ))
        {
            Debug::Throw() << "Ssh::Connection::connect - Cannot initialize session" << endl;
            _notifyError( tr( "cannot initialize ssh session" ) );
            return false;
        }

        // cast session
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );

        // initialize agent
        agent_ = libssh2_agent_init(session);
        if( !agent_ )
        {
            // do not throw error because one can connect without the agent
            Debug::Throw(0, "Ssh::Connection::connect - failed initializing agent.\n" );
        }

        // update state and return
        state_ |= SessionCreated;

        // mark as non blocking
        libssh2_session_set_blocking(session, 0);

        // request handshake and agent connection
        addCommand( Connect );
        addCommand( Handshake );
        addCommand( ConnectAgent );

        return true;
        #else
        return false;
        #endif

    }

    //_______________________________________________
    bool Connection::authenticate( bool forceRequestIdentity )
    {

        Debug::Throw( "Ssh::Connection::authenticate.\n" );

        // authentication
        if( forceRequestIdentity ) addCommand(Connection::RequestIdentity);

        addCommand(Connection::LoadAuthenticationMethods);
        addCommand(Connection::ListIdentities);
        addCommand(Connection::AuthenticateWithAgent);

        if( !( forceRequestIdentity || attributes_.rememberPassword() ) )
        { addCommand(Connection::RequestIdentity); }

        addCommand(Connection::AuthenticateWithPassword);
        return true;

    }

    //_______________________________________________________________________
    bool Connection::waitForConnected( int msecs )
    {

        // do nothing if already connected
        if( isConnected() ) return true;

        // setup host manually if not already set
        if( sshHost_.lookupId() < 0 )
        {
            sshHost_ = QHostInfo::fromName( attributes_.host() );
            sshHost_.setLookupId( 0 );
        }

        // start timer
        QElapsedTimer timer;
        timer.start();
        while( msecs < 0 || timer.elapsed() < msecs )
        {
            // qApp->processEvents();
            if( !_processCommands() ) break;
            Sleep::msleep( 100 );
        }

        return isConnected();

    }

    //_______________________________________________
    void Connection::addCommand( Command command )
    {
        Debug::Throw() << "Ssh::Connection::AddCommand: " << command << endl;
        commands_.append( command );
        if( !timer_.isActive() ) timer_.start( latency_, this );
    }

    //_______________________________________________
    void Connection::abortCommands( void )
    {
        if( timer_.isActive() ) timer_.stop();
        commands_.clear();
    }

    //_______________________________________________
    void Connection::disconnect( void )
    {

        Debug::Throw( "Ssh::Connection::disconnect.\n" );
        _disconnectChannels();
        _disconnectTunnels();
        _disconnectSession();

    }

    //_______________________________________________
    void Connection::_saveHost( QHostInfo host )
    { sshHost_ = host; }

    //_______________________________________________
    void Connection::_disconnectChannels( void )
    {

        Debug::Throw( "Ssh::Connection::_disconnectChannels.\n" );

        // loop over tunnels and delete
        for( const auto& tunnel:findChildren<Tunnel*>() )
        {
            tunnel->close();
            tunnel->deleteLater();
        }

    }

    //_______________________________________________
    void Connection::_disconnectSession( void )
    {

        Debug::Throw( "Ssh::Connection::_disconnectSession.\n" );

        #if HAVE_SSH
        if( state_ & (Connected|SessionCreated) )
        {

            // disconnect agent
            if( agent_ )
            {
                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                libssh2_agent_disconnect(agent);
                libssh2_agent_free(agent);
                agent_ = nullptr;
                identity_ = nullptr;
            };

            // close session
            if( session_ )
            {
                LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );
                libssh2_session_disconnect( session, "Client disconnecting normally" );
                libssh2_session_free( session );
                session_ = nullptr;
            }

            // close socket
            if( sshSocket_ >= 0 )
            {
                close( sshSocket_ );
                sshSocket_ = -1;
            }

            state_ &= ~(Connected|SessionCreated);

        }
        #endif

    }

    //_______________________________________________
    void Connection::_disconnectTunnels( void )
    {
        Debug::Throw( "Ssh::Connection::_disconnectTunnel.\n" );

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
    bool Connection::isSupported( void )
    {
        #if HAVE_SSH
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

            if( !_processCommands() && timer_.isActive() ) timer_.stop();

        } else {

            return QObject::timerEvent( event );

        }

    }

    //_______________________________________________
    bool Connection::_processCommands( void )
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
            _notifyMessage( _commandMessage( lastCommand_ ) );
        }

        #if HAVE_SSH

        Debug::Throw() << "Ssh::Connection::_processCommands - processing command: " << _commandMessage(commands_.front()) << " (" << commands_.front() << ")" << endl;

        // cast session. It is used for almost all commands
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );
        switch( commands_.front() )
        {

            case Connect:
            {

                // check host
                if( sshHost_.lookupId() < 0 )
                {
                    Debug::Throw() << "Ssh::Connection::_processCommands - lookup failed" << endl;
                    break;
                }

                // initialize socket address structure
                QHostAddress address;
                if( !sshHost_.addresses().isEmpty() ) address = sshHost_.addresses().front();
                if( address.isNull() )
                {

                    _abortCommands( tr( "Invalid host: %1" ).arg( attributes_.host() ) );
                    return false;

                }

                Debug::Throw() << "Ssh::Connection::_processCommands - connection."
                    << " Host: " << attributes_.host()
                    << " Port: " << attributes_.port()
                    << endl;

                // initialize socket address
                struct sockaddr_in socketAddress;
                socketAddress.sin_family = AF_INET;
                socketAddress.sin_port = htons(attributes_.port());
                socketAddress.sin_addr.s_addr = htonl(address.toIPv4Address());

                // try connect
                const int result( ::connect( sshSocket_, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(struct sockaddr_in) ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( errno != EALREADY && errno != EINPROGRESS ) {

                    _abortCommands( tr( "Cannot connect to host %1:%2 - %3" )
                        .arg(attributes_.host())
                        .arg(attributes_.port())
                        .arg(errno) );
                    return false;

                }

            }
            break;


            case Handshake:
            {
                const int result( libssh2_session_handshake( session, sshSocket_ ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( result != LIBSSH2_ERROR_EAGAIN ) {

                    _abortCommands( tr( "Handshake failed" ) );
                    return false;

                }

            }
            break;

            case ConnectAgent:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return true;

            } else {

                // accept in all cases except EAGAIN
                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_connect(agent) != LIBSSH2_ERROR_EAGAIN  )
                {

                    commands_.removeFirst();
                    return true;

                }

            }
            break;

            case RequestIdentity:
            {
                // login dialog
                LoginDialog dialog( nullptr );
                dialog.setAttributes( attributes_ );
                if( dialog.exec() )
                {

                    // get updated attributes
                    const ConnectionAttributes attributes( dialog.attributes() );

                    // detect changes, save, and emit signal if needed
                    if( ( attributes_.userName() != attributes.userName() ) ||
                        ( attributes_.password() != attributes.password() ) ||
                        ( attributes_.rememberPassword() != attributes.rememberPassword() ) )
                    {
                        attributes_ = attributes;
                        emit attributesChanged();
                    }

                    commands_.removeFirst();
                    return true;

                } else {

                    _abortCommands( tr( "Login cancelled" ) );
                    return false;

                }

            }
            break;

            case LoadAuthenticationMethods:
            {
                if( !(authenticationMethods_ = libssh2_userauth_list( session, qPrintable( attributes_.userName() ), attributes_.userName().size() )).isNull() )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN ) {

                    _abortCommands( tr( "Failed retrieving authentication methods" ) );
                    return false;

                }

            }
            break;

            case ListIdentities:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return true;

            } else if( !authenticationMethods_.contains( "publickey" ) ) {

                Debug::Throw( 0, "Ssh::Connection::_processCommands - public key authentication is not supported.\n" );
                commands_.removeFirst();
                return true;

            } else {

                // accept in all cases except EAGAIN
                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_list_identities( agent ) != LIBSSH2_ERROR_EAGAIN  )
                {

                    identity_ = nullptr;
                    commands_.removeFirst();
                    return true;

                }

            }
            break;

            case AuthenticateWithAgent:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return true;

            } else if( !authenticationMethods_.contains( "publickey" ) ) {

                Debug::Throw( 0, "Ssh::Connection::_processCommands - public key authentication is not supported.\n" );
                commands_.removeFirst();
                return true;

            } else {

                Debug::Throw() << "Ssh::Connection::_processCommands - identity: " << identity_ << endl;

                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                struct libssh2_agent_publickey* identity( reinterpret_cast<struct libssh2_agent_publickey*>( identity_ ) );
                if( identity )
                {

                    // cast
                    const int result( libssh2_agent_userauth( agent, qPrintable( attributes_.userName() ), identity ) );
                    if( !result )
                    {

                        // success
                        commands_.removeFirst();
                        state_ |= Connected;
                        commands_.clear();
                        emit connected();
                        return false;

                    } else if( result == LIBSSH2_ERROR_EAGAIN ) {

                        // try again
                        break;

                    } else {

                        Debug::Throw()
                            << "Ssh::Connection::authenticateWithAgent -"
                            << " User: " << attributes_.userName()
                            << " Key: " << identity->comment << " failed"
                            << endl;

                    }

                }

                // load next identity
                struct libssh2_agent_publickey* nextIdentity;
                if( libssh2_agent_get_identity( agent, &nextIdentity, identity ) )
                {
                    identity_ = nullptr;
                    commands_.removeFirst();
                    return true;

                } else identity_ = nextIdentity;

            }
            break;

            case AuthenticateWithPassword:
            if( !authenticationMethods_.contains( "password" ) )
            {

                // check authentication methods
                Debug::Throw( 0, "Ssh::Connection::_processCommands - password authentication is not supported.\n" );
                commands_.removeFirst();
                return true;

            } else {

                Debug::Throw() << "Ssh::Connection::_processCommands - password authentication."
                    << " Host: " << attributes_.host()
                    << " User: " << attributes_.userName()
                    << " Password: " << attributes_.password()
                    << endl;

                const int result( libssh2_userauth_password( session, qPrintable( attributes_.userName() ), qPrintable( attributes_.password() ) ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();

                    return false;

                }  else if( result != LIBSSH2_ERROR_EAGAIN ) {

                    commands_.clear();
                    emit loginFailed();

                    // start over
                    _disconnectSession();
                    connect();
                    authenticate( true );
                    return true;

                }

            }
            break;

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
    void Connection::_notifyError( QString error )
    {
        Debug::Throw(0) << "Ssh::Connection::_notifyError: " << error << endl;
        error_ = error;
        emit this->error( error );
    }

    //_______________________________________________
    void Connection::_notifyMessage( QString message )
    {
        Debug::Throw() << "Ssh::Connection::_notifyMessage: " << message << endl;
        emit this->message( message );
    }

    //_______________________________________________
    void Connection::_notifyDebug( QString message )
    { Debug::Throw() << message << endl; }

    //_______________________________________________
    void Connection::_newConnection( void )
    {
        Debug::Throw( "Ssh::Connection::_newConnection.\n" );

        // check session
        if( !session_ )
        {
            Debug::Throw(0, "Ssh::Connection::_newConnection - invalid session.\n" );
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
            auto iter( attributes_.tunnels().constFind( attributes ) );
            if( iter == attributes_.tunnels().end() )
            {
                Debug::Throw(0) << "Ssh::Connection::_newConnection - unable to find tunnel attributes matching port " << tcpServer->serverPort() << endl;
                continue;
            }

            while( QTcpSocket* tcpSocket = tcpServer->nextPendingConnection() )
            {
                Tunnel* tunnel = new Tunnel( this, tcpSocket );
                tunnel->sshSocket()->connectToHost( session_, iter->host(), iter->remotePort() );

                connect( tunnel, SIGNAL(error(QString)), this, SLOT(_notifyError(QString)) );
                connect( tunnel, SIGNAL(debug(QString)), this, SLOT(_notifyDebug(QString)) );
                connect( tunnel->sshSocket(), SIGNAL(error(QAbstractSocket::SocketError)), tunnel, SLOT(deleteLater()) );
                connect( tunnel->sshSocket(), SIGNAL(readChannelFinished()), tunnel, SLOT(deleteLater()) );
            }

        }

    }

    //_______________________________________________
    QString Connection::_commandMessage( Command command ) const
    {
        switch( command )
        {
            case Connect: return tr( "Connecting to host" );
            case Handshake: return tr( "Performing SSH handshake" );
            case ConnectAgent: return tr( "Connecting to SSH agent" );
            case RequestIdentity: return tr( "Waiting for user authentication" );
            case LoadAuthenticationMethods: return tr( "Loading authentication methods" );
            case ListIdentities: return tr( "Loading existing identities" );
            case AuthenticateWithAgent: return tr( "Trying to authenticate using SSH agent" );
            case AuthenticateWithPassword: return tr( "Trying to authenticate using password" );
            default: return QString();
        }



    }

    //_______________________________________________
    void Connection::_abortCommands( const QString& errorMessage )
    {
        Debug::Throw( "Ssh::Connection::_abortCommands.\n" );
        if( timer_.isActive() ) timer_.stop();
        commands_.clear();
        _notifyError( errorMessage );
    }

}

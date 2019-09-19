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

#include "CppUtil.h"
#include "Debug.h"
#include "File.h"
#include "Sleep.h"
#include "SshLoginDialog.h"
#include "SshSingleton.h"
#include "SshSocket.h"
#include "SshTunnel.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QTcpServer>
#include <QTextStream>
#include <QTimer>

#if defined(Q_OS_WIN)
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <unistd.h>
#include <fcntl.h>

#if WITH_SSH
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
    Connection::~Connection()
    { disconnect(); }

    //_______________________________________________
    bool Connection::createTunnels()
    {

        Debug::Throw( "Ssh::Connection::createTunnels.\n" );
        if( !Singleton::get().initialized() ) return false;

        // loop over tunnel attributes
        for( const auto& attributes:attributes_.tunnels() )
        {

            // create Tcp server
            auto tcpServer = new QTcpServer( this );
            connect( tcpServer, SIGNAL(newConnection()), SLOT(_newConnection()) );
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
    bool Connection::connect()
    {

        Debug::Throw( "Ssh::Connection::connect.\n" );
        if( !Singleton::get().initialized() ) return false;

        #if WITH_SSH
        // initialize socket
        sshSocket_ = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if( sshSocket_ == -1 )
        {
            _notifyError( tr( "unable to create ssh socket" ) );
            return false;
        }

        // make socket non blocking
        #if !defined(Q_OS_WIN)
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
        #endif

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
        auto session( static_cast<LIBSSH2_SESSION*>(session_) );

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
        addCommand( Command::Connect );
        addCommand( Command::Handshake );
        addCommand( Command::ConnectAgent );

        return true;
        #else
        return false;
        #endif

    }

    //_______________________________________________
    bool Connection::authenticate( bool forceRequestIdentity )
    {

        Debug::Throw() << "Ssh::Connection::authenticate - forceRequestIdentity: " << forceRequestIdentity << endl;

        // authentication
        if( forceRequestIdentity ) addCommand(Command::RequestIdentity);

        addCommand(Command::LoadAuthenticationMethods);
        addCommand(Command::ListIdentities);
        addCommand(Command::AuthenticateWithAgent);

        if( !( forceRequestIdentity || attributes_.rememberPassword() ) )
        { addCommand(Command::RequestIdentity); }

        addCommand(Command::AuthenticateWithPassword);
        return true;

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

        // setup host manually if not already set
        if( sshHost_.lookupId() < 0 )
        {
            sshHost_ = QHostInfo::fromName( attributes_.host() );
            sshHost_.setLookupId( 0 );
        }

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
    void Connection::addCommand( Command command )
    {
        Debug::Throw() << "Ssh::Connection::AddCommand: " << _commandMessage( command ) << endl;
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

        Debug::Throw( "Ssh::Connection::disconnect.\n" );
        _disconnectChannels();
        _disconnectTunnels();
        _disconnectSession();

    }

    //_______________________________________________
    void Connection::_saveHost( QHostInfo host )
    { sshHost_ = host; }

    //_______________________________________________
    void Connection::_disconnectChannels()
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
    void Connection::_disconnectSession()
    {

        Debug::Throw( "Ssh::Connection::_disconnectSession.\n" );

        #if WITH_SSH
        if( state_ & (Connected|SessionCreated) )
        {

            // disconnect agent
            if( agent_ )
            {
                auto agent( static_cast<LIBSSH2_AGENT*>(agent_) );
                libssh2_agent_disconnect(agent);
                libssh2_agent_free(agent);
                agent_ = nullptr;
                identity_ = nullptr;
            };

            // close session
            if( session_ )
            {
                auto session( static_cast<LIBSSH2_SESSION*>(session_) );
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
    void Connection::_disconnectTunnels()
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

        } else {

            return QObject::timerEvent( event );

        }

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
            _notifyMessage( _commandMessage( lastCommand_ ) );
        }

        #if WITH_SSH

        Debug::Throw() << "Ssh::Connection::_processCommands - processing command: " << _commandMessage(commands_.front()) << endl;

        // cast session. It is used for almost all commands
        auto session( static_cast<LIBSSH2_SESSION*>(session_) );
        switch( commands_.front() )
        {

            case Command::Connect:
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
                auto result( ::connect( sshSocket_, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(struct sockaddr_in) ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( errno != EALREADY && errno != EINPROGRESS && errno != EWOULDBLOCK && errno != EAGAIN ) {

                    _abortCommands( tr( "Cannot connect to host %1:%2 - %3" )
                        .arg(attributes_.host())
                        .arg(attributes_.port())
                        .arg(strerror( errno ) ) );
                    return false;

                }

            }
            break;


            case Command::Handshake:
            {
                const auto result( libssh2_session_handshake( session, sshSocket_ ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    return true;

                } else if( result != LIBSSH2_ERROR_EAGAIN ) {

                    _abortCommands( tr( "Handshake failed - %1" ).arg(  _sshErrorString( result ) ) );
                    return false;

                }

            }
            break;

            case Command::ConnectAgent:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return true;

            } else {

                // accept in all cases except EAGAIN
                auto agent( static_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_connect(agent) != LIBSSH2_ERROR_EAGAIN  )
                {

                    commands_.removeFirst();
                    return true;

                }

            }
            break;

            case Command::RequestIdentity:
            {

                // login dialog
                LoginDialog dialog( nullptr );
                dialog.setAttributes( attributes_ );
                if( dialog.exec() )
                {

                    // get updated attributes
                    const auto attributes( dialog.attributes() );

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

                    _abortCommands( tr( "Login canceled" ) );
                    return false;

                }

            }
            break;

            case Command::LoadAuthenticationMethods:
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

            case Command::ListIdentities:
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
                auto agent( static_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_list_identities( agent ) != LIBSSH2_ERROR_EAGAIN  )
                {

                    identity_ = nullptr;
                    commands_.removeFirst();
                    return true;

                }

            }
            break;

            case Command::AuthenticateWithAgent:
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

                auto agent( static_cast<LIBSSH2_AGENT*>(agent_) );
                auto identity( static_cast<struct libssh2_agent_publickey*>( identity_ ) );
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

            case Command::AuthenticateWithPassword:
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
    void Connection::_newConnection()
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

            while( auto tcpSocket = tcpServer->nextPendingConnection() )
            {
                auto tunnel = new Tunnel( this, tcpSocket );
                tunnel->sshSocket()->connectToHost( session_, iter->host(), iter->remotePort() );

                connect( tunnel, SIGNAL(error(QString)), SLOT(_notifyError(QString)) );
                connect( tunnel, SIGNAL(debug(QString)), SLOT(_notifyDebug(QString)) );
                connect( tunnel->sshSocket(), SIGNAL(error(QAbstractSocket::SocketError)), tunnel, SLOT(deleteLater()) );
                connect( tunnel->sshSocket(), SIGNAL(readChannelFinished()), tunnel, SLOT(deleteLater()) );
            }

        }

    }

    //_______________________________________________
    QString Connection::_sshErrorString( int error ) const
    {
        using ErrorHash = QHash<int,QString>;
        // error codes copied from libssh2.h
        static const auto errorStrings = Base::makeT<ErrorHash>( {
            { LIBSSH2_ERROR_SOCKET_NONE, tr( "Invalid socket" ) },
            { LIBSSH2_ERROR_BANNER_RECV, tr( "Banner from remote host not received" ) },
            { LIBSSH2_ERROR_BANNER_SEND, tr( "Unable to send banner to remote host" ) },
            { LIBSSH2_ERROR_INVALID_MAC, tr( "Invalid MAC" ) },
            { LIBSSH2_ERROR_KEX_FAILURE, tr( "Encryption key exchange with the remote host failed" ) },
            { LIBSSH2_ERROR_ALLOC, tr( "Bad allocation" ) },
            { LIBSSH2_ERROR_SOCKET_SEND, tr( "Unable to send data to socket" ) },
            { LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE, tr(  "Encryption key exchange with the remote host failed" ) },
            { LIBSSH2_ERROR_TIMEOUT, tr( "Command timed out" ) },
            { LIBSSH2_ERROR_HOSTKEY_INIT, tr( "Unable to initialize host key" ) },
            { LIBSSH2_ERROR_HOSTKEY_SIGN, tr( "Unable to sign host key" ) },
            { LIBSSH2_ERROR_DECRYPT, tr( "Decryption error" ) },
            { LIBSSH2_ERROR_SOCKET_DISCONNECT, tr( "Socket is disconnected" ) },
            { LIBSSH2_ERROR_PROTO, tr( "An invalid SSH protocol response was received on the socket" ) },
            { LIBSSH2_ERROR_PASSWORD_EXPIRED, tr( "Password has expired" ) },
            { LIBSSH2_ERROR_FILE, tr( "Invalid file" ) },
            { LIBSSH2_ERROR_METHOD_NONE, tr( "Invalid authentication method" ) },
            { LIBSSH2_ERROR_AUTHENTICATION_FAILED, tr( "Authentication failed" ) },
            { LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED, tr( "Unverified public key" ) },
            { LIBSSH2_ERROR_CHANNEL_OUTOFORDER, tr( "Invalid chanel" ) },
            { LIBSSH2_ERROR_CHANNEL_FAILURE, tr( "Failed to read/write to ssh channel" ) },
            { LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED, tr( "Channel request denied" ) },
            { LIBSSH2_ERROR_CHANNEL_UNKNOWN, tr( "Channel is unknown" ) },
            { LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED, tr( "Channel window exceed" ) },
            { LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED, tr( "Channel packet exceed" ) },
            { LIBSSH2_ERROR_CHANNEL_CLOSED, tr( "Channel is closed" ) },
            { LIBSSH2_ERROR_CHANNEL_EOF_SENT, tr( "Channel has already sent EOF" ) },
            { LIBSSH2_ERROR_SCP_PROTOCOL, tr( "Invalid scp protocol" ) },
            { LIBSSH2_ERROR_ZLIB, tr( "ZLib failed" ) },
            { LIBSSH2_ERROR_SOCKET_TIMEOUT, tr( "Socket timeout" ) },
            { LIBSSH2_ERROR_SFTP_PROTOCOL, tr( "Invalid sftp protocol" ) },
            { LIBSSH2_ERROR_REQUEST_DENIED, tr( "Request denied" ) },
            { LIBSSH2_ERROR_METHOD_NOT_SUPPORTED, tr( "Method not supported" ) },
            { LIBSSH2_ERROR_INVAL, tr( "Invalid arguments" ) },
            { LIBSSH2_ERROR_INVALID_POLL_TYPE, tr( "Invalid poll type" ) },
            { LIBSSH2_ERROR_PUBLICKEY_PROTOCOL, tr( "invalid public key protocol" ) },
            { LIBSSH2_ERROR_EAGAIN, tr( "Non blocking" ) },
            { LIBSSH2_ERROR_BUFFER_TOO_SMALL, tr( "Buffer is too small" ) },
            { LIBSSH2_ERROR_BAD_USE, tr( "Bad use" ) },
            { LIBSSH2_ERROR_COMPRESS, tr( "Compression error" ) },
            { LIBSSH2_ERROR_OUT_OF_BOUNDARY, tr( "Out of bound" ) },
            { LIBSSH2_ERROR_AGENT_PROTOCOL, tr( "Invalid agent protocol" ) },
            { LIBSSH2_ERROR_SOCKET_RECV, tr( "Unable to read data from socket" ) },
            { LIBSSH2_ERROR_ENCRYPT, tr( "Encryption error" ) },
            { LIBSSH2_ERROR_BAD_SOCKET, tr( "Socket is in a bad state" ) },
            { LIBSSH2_ERROR_KNOWN_HOSTS, tr( "Known hosts error" ) }
        });

        auto iter = errorStrings.find( error );
        return iter == errorStrings.end() ? tr( "Unknown error - %1" ).arg( error ) : iter.value();
    }

    //_______________________________________________
    QString Connection::_commandMessage( Command command ) const
    {
        using CommandHash = QHash<Command,QString>;
        static const auto commandNames = Base::makeT<CommandHash>( {
            { Command::Connect, tr( "Connecting to host" ) },
            { Command::Handshake, tr( "Performing SSH handshake" ) },
            { Command::ConnectAgent, tr( "Connecting to SSH agent" ) },
            { Command::RequestIdentity, tr( "Waiting for user authentication" ) },
            { Command::LoadAuthenticationMethods, tr( "Loading authentication methods" ) },
            { Command::ListIdentities, tr( "Loading existing identities" ) },
            { Command::AuthenticateWithAgent, tr( "Trying to authenticate using SSH agent" ) },
            { Command::AuthenticateWithPassword, tr( "Trying to authenticate using password" ) }
        });

        return commandNames[command];
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

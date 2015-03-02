#include "SshConnection.h"

#include "Debug.h"
#include "File.h"
#include "SshChannelThread.h"
#include "SshListeningThread.h"
#include "SshLoginDialog.h"
#include "Util.h"

#include <QHostInfo>
#include <QTextStream>

#if HAVE_SSH
#include <libssh2.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

namespace Ssh
{

    //_______________________________________________
    Connection::Connection( QObject* parent ):
        QObject( parent ),
        Counter( "Ssh::Connection" )
    {
        Debug::Throw( "Ssh::Connection::Connection.\n" );

        #if HAVE_SSH
        if( !libssh2_init(0) ) state_ |= Initialized;
        #endif
    }

    //_______________________________________________
    Connection::~Connection( void )
    {
        Debug::Throw( "Ssh::Connection::~Connection.\n" );
        if( state_ & Initialized )
        {
            disconnect();
            #if HAVE_SSH
            libssh2_exit();
            #endif
        }
    }

    //_______________________________________________
    bool Connection::createTunnels( void )
    {

        Debug::Throw( 0, "Ssh::Connection::createTunnels.\n" );
        if( !(state_ & Initialized) ) return false;

        // loop over tunnel attributes
        foreach( auto attributes, attributes_.tunnels() )
        {

            ListeningThread* thread = new ListeningThread( this, attributes );
            connect( thread, SIGNAL(error(QString)), this, SLOT(_notifyError(QString)) );
            connect( thread, SIGNAL(newConnection(int,int)), this, SLOT(_newConnection(int,int)) );
            connect( thread, SIGNAL(finished()), thread, SLOT(close()) );
            connect( thread, SIGNAL(finished()), thread, SLOT(deleteLater()) );
            thread->initialize();
            thread->start();
        }

        // update state and return
        state_ |= TunnelCreated;

        return true;

    }

    //_______________________________________________
    bool Connection::connect( void )
    {

        Debug::Throw( 0, "Ssh::Connection::connect.\n" );
        if( !(state_ & Initialized) ) return false;

        #if HAVE_SSH
        // initialize socket
        sshSocket_ = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if( sshSocket_ == -1 )
        {
            perror( "socket" );
            return false;
        }

        // initialize socket address structure
        QHostAddress address;
        const QHostInfo hostInfo( QHostInfo::fromName( attributes_.host() ) );
        if( !hostInfo.addresses().isEmpty() ) address = hostInfo.addresses().front();
        if( address.isNull() )
        {
            Debug::Throw(0) << "Ssh::Connection::connect - invalid host: " << attributes_.host() << endl;
            return false;
        }

        struct sockaddr_in socketAddress;
        socketAddress.sin_family = AF_INET;
        socketAddress.sin_port = htons(attributes_.port());
        socketAddress.sin_addr.s_addr = htonl(address.toIPv4Address());

        // connect
        if( ::connect( sshSocket_, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(struct sockaddr_in) ) )
        {
            Debug::Throw(0, "Ssh::Connection::connect - failed to connect.\n" );
            return false;
        }

        // session
        if(!( session_ = libssh2_session_init() ))
        {
            Debug::Throw(0, "Ssh::Connection::connect - Could not initialize SSH session.\n" );
            return false;
        }

        // cast session
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );

        // initialize agent
        agent_ = libssh2_agent_init(session);
        if( !agent_ ) Debug::Throw(0, "Ssh::Connection::connect - failed initializing agent.\n" );

        // mark as non blocking
        libssh2_session_set_blocking(session, 0);

        // request handshake and agent connection
        addCommand( Handshake );
        addCommand( ConnectAgent );

        // update state and return
        state_ |= SessionCreated;
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

    //_______________________________________________
    void Connection::addCommand( SshCommand command )
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

        Debug::Throw(0, "Ssh::Connection::disconnect.\n" );
        disconnectChannels();
        disconnectSession();
        disconnectTunnels();

    }

    //_______________________________________________
    void Connection::disconnectChannels( void )
    {

        Debug::Throw(0, "Ssh::Connection::disconnectChannels.\n" );

        // close ssh tunnels
        foreach( auto thread, findChildren<ChannelThread*>() )
        {
            thread->terminate();
            thread->wait();
        }

    }

    //_______________________________________________
    void Connection::disconnectSession( void )
    {

        Debug::Throw(0, "Ssh::Connection::disconnectSession.\n" );

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
    void Connection::disconnectTunnels( void )
    {
        Debug::Throw(0, "Ssh::Connection::disconnectTunnel.\n" );

        // disconnect all tcp servers
        foreach( auto thread, findChildren<ListeningThread*>() )
        {
            thread->terminate();
            thread->wait();
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

        if( event->timerId() != timer_.timerId() )
        { return QObject::timerEvent( event ); }

        // check empty commands list
        if( commands_.empty() )
        {
            timer_.stop();
            return;
        }

        // check session
        if( !session_ )
        {
            _abortCommands( tr( "Session is invalid" ) );
            return;
        }

        // emit message
        if( commands_.front() != lastCommand_ )
        {
            lastCommand_ = commands_.front();
            emit message( _commandMessage( lastCommand_ ) );
        }

        #if HAVE_SSH

        Debug::Throw() << "Ssh::Connection::timerEvent - processing command: " << _commandMessage(commands_.front()) << " (" << commands_.front() << ")" << endl;

        // cast session. It is used for almost all commands
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );
        switch( commands_.front() )
        {

            case Handshake:
            {
                const int result( libssh2_session_handshake( session, sshSocket_ ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    return;

                } else if( result != LIBSSH2_ERROR_EAGAIN ) {

                    _abortCommands( tr( "Handshake failed" ) );
                    return;

                }

            }
            break;

            case ConnectAgent:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return;

            } else {

                // accept in all cases except EAGAIN
                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_connect(agent) != LIBSSH2_ERROR_EAGAIN  )
                {

                    commands_.removeFirst();
                    return;

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
                    attributes_ = dialog.attributes();
                    commands_.removeFirst();
                    return;

                } else {

                    _abortCommands( tr( "Login cancelled" ) );
                    return;

                }

            }
            break;

            case LoadAuthenticationMethods:
            {
                if( !(authenticationMethods_ = libssh2_userauth_list( session, qPrintable( attributes_.userName() ), attributes_.userName().size() )).isNull() )
                {

                    // success
                    commands_.removeFirst();
                    return;

                } else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN ) {

                    _abortCommands( tr( "Failed retrieving authentication methods" ) );
                    return;

                }

            }
            break;

            case ListIdentities:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return;

            } else if( !authenticationMethods_.contains( "publickey" ) ) {

                Debug::Throw( 0, "Ssh::Connection::timerEvent - public key authentication is not supported.\n" );
                commands_.removeFirst();
                return;

            } else {

                // accept in all cases except EAGAIN
                LIBSSH2_AGENT* agent( reinterpret_cast<LIBSSH2_AGENT*>(agent_) );
                if( libssh2_agent_list_identities( agent ) != LIBSSH2_ERROR_EAGAIN  )
                {

                    identity_ = nullptr;
                    commands_.removeFirst();
                    return;

                }

            }
            break;

            case AuthenticateWithAgent:
            if( !agent_ )
            {

                // skip if no agent is defined
                commands_.removeFirst();
                return;

            } else if( !authenticationMethods_.contains( "publickey" ) ) {

                Debug::Throw( 0, "Ssh::Connection::timerEvent - public key authentication is not supported.\n" );
                commands_.removeFirst();
                return;

            } else {

                Debug::Throw() << "Ssh::Connection::timerEvent - identity: " << identity_ << endl;

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
                        return;

                    } else if( result == LIBSSH2_ERROR_EAGAIN ) {

                        // try again
                        break;

                    } else {

                        Debug::Throw()
                            << "Ssh::Connection::authenticateWithAgent -"
                            << " login with user name: " << attributes_.userName()
                            << " and key: " << identity->comment << " failed"
                            << endl;

                    }

                }

                // load next identity
                struct libssh2_agent_publickey* nextIdentity;
                if( libssh2_agent_get_identity( agent, &nextIdentity, identity ) )
                {
                    identity_ = nullptr;
                    commands_.removeFirst();
                    return;
                } else identity_ = nextIdentity;

            }
            break;

            case AuthenticateWithPassword:
            if( !authenticationMethods_.contains( "password" ) )
            {

                // check authentication methods
                Debug::Throw( 0, "Ssh::Connection::timerEvent - password authentication is not supported.\n" );
                commands_.removeFirst();
                return;

            } else {

                const int result( libssh2_userauth_password( session, qPrintable( attributes_.userName() ), qPrintable( attributes_.password() ) ) );
                if( !result )
                {

                    // success
                    commands_.removeFirst();
                    state_ |= Connected;
                    commands_.clear();
                    emit connected();

                    return;

                }  else if( result != LIBSSH2_ERROR_EAGAIN ) {

                    commands_.clear();
                    emit loginFailed();

                    // start over
                    disconnectSession();
                    connect();
                    authenticate( true );
                    return;

                }

            }
            break;

            default:
            commands_.removeFirst();
            return;

        }

        #else

        _abortCommands( tr( "No SSH support" ) );

        #endif

    }

    //_______________________________________________
    void Connection::_notifyError( QString error )
    { Debug::Throw(0) << "Ssh::Connection::_notifyError: " << error << endl; }

    //_______________________________________________
    void Connection::_newConnection( int port, int socket )
    {
        Debug::Throw( "Ssh::Connection::_newConnection.\n" );

        // loop over listening threads
        bool found = false;
        foreach( auto thread, findChildren<ListeningThread*>() )
        {

            if( thread->isRunning() && thread->attributes().localPort() == port )
            {
                found = true;
                const TunnelAttributes& attributes( thread->attributes() );
                Debug::Throw(0) << "Ssh::Connection::_newConnection - new connection to " << attributes.host() << ":" << attributes.remotePort() << endl;
                ChannelThread* channelThread= new ChannelThread( this, attributes, session_, socket );
                connect( channelThread, SIGNAL(error(QString)), this, SLOT(_notifyError(QString)) );
                connect( channelThread, SIGNAL(finished()), channelThread, SLOT(close()) );
                connect( channelThread, SIGNAL(finished()), channelThread, SLOT(deleteLater()) );
                channelThread->initialize();
                channelThread->start();
            }
        }

        if( !found )
        { Debug::Throw(0) << "Could not create ssh channel on port: " << port << endl; }

    }

    //_______________________________________________
    QString Connection::_commandMessage( SshCommand command ) const
    {
        switch( command )
        {
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
        emit error( errorMessage );
    }

}

// $Id$

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

#include "ApplicationManager.h"
#include "ApplicationManager.moc"
#include "Client.h"
#include "Debug.h"
#include "Util.h"
#include "XmlOptions.h"

#include <QCoreApplication>
#include <algorithm>

namespace Server
{

    //_________________________________________
    ApplicationManager::ApplicationManager( QObject* parent ):
        QObject( parent ),
        Counter( "ApplicationManager" ),
        host_( QHostAddress::LocalHost ),
        port_( 8082 ),
        server_( new QTcpServer( this ) ),
        serverInitialized_( false ),
        client_( new Client( this, new QTcpSocket( this ) ) ),
        state_( AwaitingReply )
    {

        Debug::Throw( "ApplicationManager::ApplicationManager.\n" );
        setApplicationName( "Generic Application" );

        connect( &_server(), SIGNAL(newConnection()), SLOT(_newConnection()) );

        // create new socket
        connect( &client().socket(), SIGNAL(error(QAbstractSocket::SocketError)), SLOT(_error(QAbstractSocket::SocketError)) );
        connect( &client().socket(), SIGNAL(connected()), SLOT(_startTimer()) );
        connect( &client().socket(), SIGNAL(disconnected()), SLOT(_serverConnectionClosed()) );
        connect( &client(), SIGNAL(commandAvailable(Server::ServerCommand)), SLOT(_process(Server::ServerCommand)) );

        if( !XmlOptions::get().contains( "SERVER_HOST" ) )
        { XmlOptions::get().setRaw( "SERVER_HOST", QHostAddress( QHostAddress::LocalHost ).toString(), true ); }

        if( !XmlOptions::get().contains( "SERVER_PORT" ) )
        { XmlOptions::get().set<int>( "SERVER_PORT", 8090, true ); }

    }

    //_________________________________________
    ApplicationManager::~ApplicationManager( void )
    {
        Debug::Throw( "ApplicationManager::~ApplicationManager.\n" );
        delete client_;
        delete server_;
    }

    //_________________________________________
    CommandLineParser ApplicationManager::commandLineParser( CommandLineArguments arguments, bool ignoreWarnings )
    {

        Debug::Throw() << "ApplicationManager::commandLineParser" << endl;

        CommandLineParser out;
        out.setGroup( CommandLineParser::serverGroupName );
        out.registerFlag( "--replace", tr( "replace existing application instance with new one" ) );
        out.registerFlag( "--no-server", tr( "ignore server mode. Run new application instance" ) );
        out.registerFlag( "--abort", tr( "exit existing instance" ) );
        out.registerOption( "--server-host", tr( "string" ), tr( "use specified host for server communication" ) );
        out.registerOption( "--server-port", tr( "integer" ), tr( "use specified port for server communication" ) );

        if( !arguments.isEmpty() )
        { out.parse( arguments, ignoreWarnings ); }

        return out;

    }

    //_____________________________________________________
    void ApplicationManager::initialize( CommandLineArguments arguments )
    {

        Debug::Throw( "ApplicationManager::init.\n" );

        // store arguments
        _setArguments( arguments );

        // overwrite host from command line arguments
        CommandLineParser parser( commandLineParser( _arguments() ) );
        if( parser.hasOption( "--server-host" ) )
        {

            QString host( parser.option( "--server-host" ) );
            XmlOptions::get().setRaw( "SERVER_HOST", host );
            _setHost( QHostAddress( host ) );

        } else _setHost( QHostAddress( QString( XmlOptions::get().raw( "SERVER_HOST" ) ) ) );

        // overwrite port from command line arguments
        if( parser.hasOption( "--server-port" ) )
        {

            unsigned int port( parser.option( "--server-port" ).toUInt() );
            XmlOptions::get().set<unsigned int>( "SERVER_PORT", port );
            _setPort( port );

        } else _setPort( XmlOptions::get().get<unsigned int>( "SERVER_PORT" ) );

        Debug::Throw() << "ApplicationManager::initialize - port: " << _port() << endl;
        _initializeClient();

        Debug::Throw( "ApplicationManager::init. done.\n" );

    }

    //_____________________________________________________
    void ApplicationManager::setApplicationName( const QString& name )
    {
        Debug::Throw() << "ApplicationManager::setApplicationName - " << name << endl;
        id_ = ApplicationId( name );
        id_.setProcessId( QCoreApplication::applicationPid() );
    }

    //______________________________________________________________
    void ApplicationManager::timerEvent(QTimerEvent *event)
    {

        if (event->timerId() == timer_.timerId() )
        {

            Debug::Throw( "ApplicationManager::timerEvent.\n" );
            timer_.stop();

            // the timer is triggered only when the client is connected
            // its expiration means it could not recieve acceptation/denial
            // from the server it is connected to.
            // the application is then set to Alive
            if( state_ == AwaitingReply && setState( Alive ) )
            { emit commandRecieved( ServerCommand( id_, ServerCommand::Accepted ) ); }

        }

        return QObject::timerEvent( event );

    }

    //_____________________________________________________
    ApplicationManager::ClientMap::iterator ApplicationManager::_register( const ApplicationId& id, Client* client, bool forced )
    {
        Debug::Throw( "ApplicationManager::_register.\n" );

        if( forced ) return acceptedClients_.insert( id, client );
        else {

            ClientMap::iterator iter( acceptedClients_.find( id ) );
            if( iter == acceptedClients_.end() ) return acceptedClients_.insert( id, client );
            else return iter;

        }
    }

    //_____________________________________________________
    void ApplicationManager::_redirect( ServerCommand command, Client* sender )
    {

        Debug::Throw() << "ApplicationManager::_redirect -"
            << " application:" << command.id().name()
            << " command: " << command.commandName()
            << endl;

        CommandLineParser parser( commandLineParser( command.arguments() ) );
        switch( command.command() )
        {

            case ServerCommand::Unlock:
            {

                // unlock request. Clear list of registered applications
                acceptedClients_.clear();
                return;

            }

            case ServerCommand::Request:
            {

                // server request
                ClientMap::iterator clientIterator( _register( command.id(), sender ) );
                Client* existingClient( clientIterator.value() );

                if( sender == existingClient )
                {

                    if( parser.hasFlag( "--abort" ) )
                    {

                        // abort existing client
                        sender->sendCommand( ServerCommand( command.id(), ServerCommand::Denied ) );
                        _broadcast( ServerCommand( command.id(), ServerCommand::Identify ), sender );
                        _register( command.id(), sender, true );

                    } else {

                        // tell client it is accepted
                        sender->sendCommand( ServerCommand( command.id(), ServerCommand::Accepted ) );
                        _broadcast( ServerCommand( command.id(), ServerCommand::Identify ), sender );

                    }

                } else if( parser.hasFlag( "--replace" ) ) {

                    // tell existing client to die
                    ServerCommand abortCommand( command.id(), ServerCommand::Abort );
                    existingClient->sendCommand( abortCommand );
                    _broadcast( ServerCommand( command.id(), ServerCommand::Killed ), existingClient );

                    // tell new client it is accepted
                    sender->sendCommand( ServerCommand( command.id(), ServerCommand::Accepted ) );
                    _broadcast( ServerCommand( command.id(), ServerCommand::Identify ), sender );
                    _register( command.id(), sender, true );

                    // update iterator pid
                    const_cast<ApplicationId*>( &clientIterator.key() )->setProcessId( command.id().processId() );

                } else if( parser.hasFlag( "--abort" ) ) {

                    // tell existing client to die
                    ServerCommand abortCommand( command.id(), ServerCommand::Abort );
                    existingClient->sendCommand( abortCommand );
                    _broadcast( ServerCommand( command.id(), ServerCommand::Killed ), existingClient );

                    // tell new client it is denied too
                    sender->sendCommand( ServerCommand( command.id(), ServerCommand::Denied ) );
                    _broadcast( ServerCommand( command.id(), ServerCommand::Identify ), sender );
                    _register( command.id(), sender, true );

                } else {

                    // tell existing client to raise itself
                    ServerCommand raiseCommand( command.id(), ServerCommand::Raise );
                    raiseCommand.setArguments( command.arguments() );
                    existingClient->sendCommand( raiseCommand );

                }

                return;
            }

            case ServerCommand::Alive:
            {
                // client exist and is alive. Deny current
                _broadcast( ServerCommand( command.id(), ServerCommand::Denied ) );
                return;

            }

            case ServerCommand::Identify:
            {
                /*
                identification request. Loop over registered clients
                send the associated Identity to the sender
                */

                for( ClientMap::iterator it=acceptedClients_.begin(); it!=acceptedClients_.end(); it++ )
                { sender->sendCommand( ServerCommand( it.key(), ServerCommand::Identify ) ); }

                // identify the server
                sender->sendCommand( ServerCommand( id_, ServerCommand::IdentifyServer ) );
                return;
            }

            default:
            {
                // redirect unrecognized message to all clients but the sender
                _broadcast( command, sender );
                return;
            }

        }

    }

    //_____________________________________________________
    void ApplicationManager::_broadcast( ServerCommand command, Client* sender )
    {

        Debug::Throw() << "ApplicationManager::_Broadcast - id: " << command.id().name() << " command: " << command.commandName() << endl;
        for( ClientList::iterator iter = _connectedClients().begin(); iter != _connectedClients().end(); ++iter )
        { if( (*iter) != sender ) (*iter)->sendCommand( command ); }

    }

    //_____________________________________________________
    void ApplicationManager::_newConnection()
    {
        Debug::Throw( "ApplicationManager::_newConnection.\n" );

        // check pending connection
        if( !_server().hasPendingConnections() ) return;

        // create client from pending connection
        Client *client( new Client( this, _server().nextPendingConnection() ) );
        connect( client, SIGNAL(commandAvailable(Server::ServerCommand)), SLOT(_redirect(Server::ServerCommand)) );
        connect( &client->socket(), SIGNAL(disconnected()), SLOT(_clientConnectionClosed()) );
        _connectedClients() << client;

    }

    //_____________________________________________________
    void ApplicationManager::_serverConnectionClosed( void )
    {
        Debug::Throw( "ApplicationManager::_serverConnectionClosed - lost connection to server.\n" );
        _setServerInitialized( false );
        initialize();
    }

    //_____________________________________________________
    void ApplicationManager::_clientConnectionClosed( void )
    {

        Debug::Throw( "ApplicationManager::_clientConnectionClosed - client has disconnected.\n" );

        // look for disconnected clients in client map
        {
            ClientMap::iterator iter;
            while( ( iter = std::find_if(  _acceptedClients().begin(), _acceptedClients().end(), SameStateFTor( QAbstractSocket::UnconnectedState ) )  ) != _acceptedClients().end() )
            {

                // broadcast client as dead
                _broadcast( ServerCommand( iter.key(), ServerCommand::Killed ), iter.value() );

                // erase from map of accepted clients
                _acceptedClients().erase( iter );

            }
        }

        // look for disconnected clients in connected clients list
        {
            ClientList::iterator iter;
            while( ( iter = std::find_if( _connectedClients().begin(), _connectedClients().end(), SameStateFTor( QAbstractSocket::UnconnectedState ) ) ) != _connectedClients().end() )
            {
                (*iter)->deleteLater();
                _connectedClients().erase( iter );
            }
        }

        return;

    }

    //_____________________________________________________
    void ApplicationManager::_error( QAbstractSocket::SocketError error )
    {

        Debug::Throw() << "ApplicationManager::_error - error:" << client().socket().errorString() << endl;

        if( error == QAbstractSocket::ConnectionRefusedError )
        {
            // stop timeout signal
            timer_.stop();

            // do nothing if client has already been denied connection
            if( state_ == Dead ) return;

            // try initialize server
            if( !_serverInitialized() )
            {

                _initializeServer();
                _initializeClient();

            } else if( setState( Alive  ) ) {

                emit commandRecieved( ServerCommand( id_, ServerCommand::Accepted ) );

            }

        } else {
            Debug::Throw() << "ApplicationManager::_error - unhandled error:" << client().socket().errorString() << endl;
        }

        return;
    }

    //_____________________________________________________
    void ApplicationManager::_redirect( ServerCommand command )
    {

        Debug::Throw( "Application::_redirect.\n" );

        ClientList::iterator iter( std::find_if(  _connectedClients().begin(), _connectedClients().end(), Client::SameIdFTor( command.clientId() ) ) );
        Q_ASSERT( iter != _connectedClients().end() );
        _redirect( command, *iter );

        return;
    }


    //_____________________________________________________
    void ApplicationManager::_process( ServerCommand command )
    {

        Debug::Throw() << "ApplicationManager::_process -"
            << "  app:" << command.id().name()
            << " command: " << command.commandName()
            << endl;

        Q_ASSERT( client().id() == command.clientId() );

        // check command id is valid
        if( !command.id().isValid() ) return;

        // check id match
        if( !( command.id() == id() ) ) return;

        switch( command.command() )
        {

            case ServerCommand::Raise:
            if( state_ == Alive )
            {

                client().sendCommand( ServerCommand( id_, ServerCommand::Alive ) );
                emit commandRecieved( command );
                return;

            } else break;

            case ServerCommand::Denied:
            if( state_ == AwaitingReply )
            {
                timer_.stop();
                if( setState( Dead ) ) emit commandRecieved( command );
                return;
            } else break;


            case ServerCommand::Abort:
            if( state_ == Alive )
            {
                timer_.stop();
                if( setState( Dead ) ) emit commandRecieved( command );
                return;
            } else break;

            case ServerCommand::Accepted:
            if( state_ == AwaitingReply )
            {
                timer_.stop();
                if( setState( Alive ) ) emit commandRecieved( command );
                return;
            } else break;

            default:
            {
                emit commandRecieved( command );
                break;
            }

        }

    }

    //__________________________________________________________________________
    bool ApplicationManager::_initializeServer( void )
    {

        Debug::Throw( "ApplicationManager::_initializeServer.\n" );

        _setServerInitialized( true );

        // connect server to port
        return _server().listen( _host(), _port() );

    }


    //__________________________________________________________________________
    bool ApplicationManager::_initializeClient( void )
    {

        Debug::Throw( "ApplicationManager::_initializeClient.\n" );
        Debug::Throw() << "ApplicationManager::_initializeClient - connecting to host: " << _host().toString() << " port: " << _port() << endl;

        // connect client to port
        client().socket().abort();
        client().socket().connectToHost( _host(), _port() );

        // emit initialization signal
        emit initialized();
        setState( AwaitingReply );

        // create request command
        ServerCommand command( id(), ServerCommand::Request );

        // add command line arguments if any
        command.setArguments( _arguments() );

        // send request command
        client().sendCommand( command );

        return true;

    }

    //__________________________________________________________________________
    void ApplicationManager::_startTimer( void )
    {
        // time out delay (for existing server to reply)
        // one should really start the timer only when the client is connected
        int timeout_delay( XmlOptions::get().contains( "SERVER_TIMEOUT_DELAY" ) ? XmlOptions::get().get<int>( "SERVER_TIMEOUT_DELAY" ) : 2000 );
        timer_.start( timeout_delay, this );
    }

}

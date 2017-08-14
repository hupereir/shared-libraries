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

#include "Client.h"
#include "CppUtil.h"
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
        server_( new QTcpServer( this ) ),
        client_( new Client( this ) )
    {

        Debug::Throw( "ApplicationManager::ApplicationManager.\n" );
        setApplicationName( "Generic Application" );

        connect( server_, SIGNAL(newConnection()), SLOT(_newConnection()) );

        // create new socket
        connect( &client_->socket(), SIGNAL(error(QAbstractSocket::SocketError)), SLOT(_error(QAbstractSocket::SocketError)) );
        connect( &client_->socket(), SIGNAL(connected()), SLOT(_startTimer()) );
        connect( &client_->socket(), SIGNAL(disconnected()), SLOT(_serverConnectionClosed()) );
        connect( client_, SIGNAL(commandAvailable(Server::ServerCommand)), SLOT(_process(Server::ServerCommand)) );

        if( !XmlOptions::get().contains( "SERVER_HOST" ) )
        { XmlOptions::get().setRaw( "SERVER_HOST", host_.toString(), true ); }

        if( !XmlOptions::get().contains( "SERVER_PORT" ) )
        { XmlOptions::get().set<int>( "SERVER_PORT", port_, true ); }

    }

    //_________________________________________
    CommandLineParser ApplicationManager::commandLineParser( CommandLineArguments arguments, bool ignoreWarnings )
    {

        Debug::Throw() << "ApplicationManager::commandLineParser" << endl;

        CommandLineParser out;
        out.setGroup( CommandLineParser::serverGroupName );
        out.registerFlag( CommandLineParser::Tag( "--replace", "-r" ), tr( "replace existing application instance with new one" ) );
        out.registerFlag( CommandLineParser::Tag( "--abort", "-q" ), tr( "exit existing instance" ) );
        out.registerFlag( "--no-server", tr( "ignore server mode. Run new application instance" ) );
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
        arguments_ = arguments;

        // overwrite host from command line arguments
        auto parser = commandLineParser( arguments_ );
        if( parser.hasOption( "--server-host" ) )
        {

            QString host( parser.option( "--server-host" ) );
            host_ = QHostAddress( host );

        } else host_ = QHostAddress( QString( XmlOptions::get().raw( "SERVER_HOST" ) ) );

        // overwrite port from command line arguments
        if( parser.hasOption( "--server-port" ) )
        {

            int port( parser.option( "--server-port" ).toUInt() );
            port_ = port;

        } else port_ = XmlOptions::get().get<int>( "SERVER_PORT" );

        Debug::Throw() << "ApplicationManager::initialize - port: " << port_ << endl;
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
    ApplicationManager::ClientMap::iterator ApplicationManager::_register( const ApplicationId& id, ClientPtr client, bool forced )
    {
        Debug::Throw( "ApplicationManager::_register.\n" );

        if( forced ) return acceptedClients_.insert( id, client );
        else {

            auto iter = acceptedClients_.find( id );
            if( iter == acceptedClients_.end() )
            { iter = std::find_if( acceptedClients_.begin(), acceptedClients_.end(), SameClientFTor(client) ); }

            if( iter == acceptedClients_.end() ) return acceptedClients_.insert( id, client );
            else return iter;

        }
    }

    //_____________________________________________________
    void ApplicationManager::_redirect( ServerCommand command, ClientPtr sender )
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

                // get existing client
                ClientMap::iterator clientIterator( _register( command.id(), sender ) );
                if( clientIterator == acceptedClients_.end() ) return;

                const auto existingClient( clientIterator.value() );

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

                // identify the server
                sender->sendCommand( ServerCommand( id_, ServerCommand::IdentifyServer ) );

                /*
                identification request. Loop over registered clients
                send the associated Identity to the sender
                */

                for( auto&& it=acceptedClients_.begin(); it!=acceptedClients_.end(); it++ )
                { sender->sendCommand( ServerCommand( it.key(), ServerCommand::Identify ) ); }

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
    void ApplicationManager::_broadcast( ServerCommand command, ClientPtr sender )
    {

        Debug::Throw() << "ApplicationManager::_Broadcast - id: " << command.id().name() << " command: " << command.commandName() << endl;
        for( auto& client:connectedClients_ )
        { if( client != sender ) client->sendCommand( command ); }

    }

    //_____________________________________________________
    void ApplicationManager::_newConnection()
    {
        Debug::Throw( "ApplicationManager::_newConnection.\n" );
        while( server_->hasPendingConnections() )
        {
            // create client from pending connection
            auto client = std::make_shared<Client>( this, server_->nextPendingConnection() );
            connect( client.get(), SIGNAL(commandAvailable(Server::ServerCommand)), SLOT(_redirect(Server::ServerCommand)) );
            connect( &client->socket(), SIGNAL(disconnected()), SLOT(_clientConnectionClosed()) );
            connectedClients_.append( client );
        }

    }

    //_____________________________________________________
    void ApplicationManager::_serverConnectionClosed()
    {
        Debug::Throw( "ApplicationManager::_serverConnectionClosed - lost connection to server.\n" );
        serverInitialized_ = false;
        initialize();
    }

    //_____________________________________________________
    void ApplicationManager::_clientConnectionClosed()
    {

        Debug::Throw( "ApplicationManager::_clientConnectionClosed - client has disconnected.\n" );

        // look for disconnected clients in client map
        for( auto&& iter = acceptedClients_.begin(); iter != acceptedClients_.end(); )
        {
            if( iter.value()->socket().state() == QAbstractSocket::UnconnectedState )
            {

                // broadcast client as dead and erase from list
                _broadcast( ServerCommand( iter.key(), ServerCommand::Killed ), iter.value() );
                iter = acceptedClients_.erase( iter );

            } else ++iter;
        }

        // remove disconnected clients from connected clients list
        connectedClients_.erase( std::remove_if( connectedClients_.begin(), connectedClients_.end(), SameStateFTor( QAbstractSocket::UnconnectedState ) ), connectedClients_.end() );

        return;

    }

    //_____________________________________________________
    void ApplicationManager::_error( QAbstractSocket::SocketError error )
    {

        Debug::Throw() << "ApplicationManager::_error - error:" << client_->socket().errorString() << endl;

        if( error == QAbstractSocket::ConnectionRefusedError )
        {
            // stop timeout signal
            timer_.stop();

            // do nothing if client has already been denied connection
            if( state_ == Dead ) return;

            // try initialize server
            if( !serverInitialized_ )
            {

                _initializeServer();
                _initializeClient();

            } else if( setState( Alive  ) ) {

                emit commandRecieved( ServerCommand( id_, ServerCommand::Accepted ) );

            }

        } else {
            Debug::Throw() << "ApplicationManager::_error - unhandled error:" << client_->socket().errorString() << endl;
        }

        return;
    }

    //_____________________________________________________
    void ApplicationManager::_redirect( ServerCommand command )
    {

        Debug::Throw( "Application::_redirect.\n" );

        const auto iter( std::find_if(  connectedClients_.begin(), connectedClients_.end(), Client::SameIdFTor( command.clientId() ) ) );
        Q_ASSERT( iter != connectedClients_.end() );
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

        Q_ASSERT( client_->id() == command.clientId() );

        // check command id is valid
        if( !command.id().isValid() ) return;

        // check id match
        if( !( command.id() == id() ) ) return;

        switch( command.command() )
        {

            case ServerCommand::Raise:
            if( state_ == Alive )
            {

                client_->sendCommand( ServerCommand( id_, ServerCommand::Alive ) );
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
    bool ApplicationManager::_initializeServer()
    {

        Debug::Throw( "ApplicationManager::_initializeServer.\n" );

        serverInitialized_ = true;

        // connect server to port
        return server_->listen( host_, port_ );

    }


    //__________________________________________________________________________
    bool ApplicationManager::_initializeClient()
    {

        Debug::Throw() << "ApplicationManager::_initializeClient - connecting to host: " << host_.toString() << " port: " << port_ << endl;

        // connect client to port
        client_->socket().abort();
        client_->socket().connectToHost( host_, port_ );

        // emit initialization signal
        emit initialized();
        setState( AwaitingReply );

        // create request command
        ServerCommand command( id_, ServerCommand::Request );

        // add command line arguments if any
        command.setArguments( arguments_ );

        // send request command
        client_->sendCommand( command );

        return true;

    }

    //__________________________________________________________________________
    void ApplicationManager::_startTimer()
    {
        // time out delay (for existing server to reply)
        // one should really start the timer only when the client is connected
        int timeoutDelay( XmlOptions::get().contains( "SERVER_TIMEOUT_DELAY" ) ? XmlOptions::get().get<int>( "SERVER_TIMEOUT_DELAY" ) : 2000 );
        timer_.start( timeoutDelay, this );
    }

}

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
        Counter( QStringLiteral("ApplicationManager") )
    {

        Debug::Throw( QStringLiteral("ApplicationManager::ApplicationManager.\n") );
        setApplicationName( "Generic Application" );

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

        Debug::Throw( QStringLiteral("ApplicationManager::init.\n") );

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

        Debug::Throw( QStringLiteral("ApplicationManager::init. done.\n") );

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

            Debug::Throw( QStringLiteral("ApplicationManager::timerEvent.\n") );
            timer_.stop();

            // the timer is triggered only when the client is connected
            // its expiration means it could not recieve acceptation/denial
            // from the server it is connected to.
            // the application is then set to Alive
            if( state_ == State::AwaitingReply && setState( State::Alive ) )
            { emit commandRecieved( ServerCommand( id_, ServerCommand::CommandType::Accepted ) ); }

        }

        return QObject::timerEvent( event );

    }

    //_____________________________________________________
    ApplicationManager::ClientMap::iterator ApplicationManager::_register( const ApplicationId& id, ClientPtr client, bool forced )
    {
        Debug::Throw( QStringLiteral("ApplicationManager::_register.\n") );

        if( forced ) return acceptedClients_.insert( id, client );
        else {

            auto iter = acceptedClients_.find( id );
            if( iter == acceptedClients_.end() ) iter = std::find( acceptedClients_.begin(), acceptedClients_.end(), client );
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

            case ServerCommand::CommandType::Unlock:
            {

                // unlock request. Clear list of registered applications
                acceptedClients_.clear();
                return;

            }

            case ServerCommand::CommandType::Request:
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
                        sender->sendCommand( ServerCommand( command.id(), ServerCommand::CommandType::Denied ) );
                        _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Identify ), sender );
                        _register( command.id(), sender, true );

                    } else {

                        // tell client it is accepted
                        sender->sendCommand( ServerCommand( command.id(), ServerCommand::CommandType::Accepted ) );
                        _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Identify ), sender );

                    }

                } else if( parser.hasFlag( "--replace" ) ) {

                    // tell existing client to die
                    ServerCommand abortCommand( command.id(), ServerCommand::CommandType::Abort );
                    existingClient->sendCommand( abortCommand );
                    _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Killed ), existingClient );

                    // tell new client it is accepted
                    sender->sendCommand( ServerCommand( command.id(), ServerCommand::CommandType::Accepted ) );
                    _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Identify ), sender );
                    _register( command.id(), sender, true );

                    // update iterator pid
                    const_cast<ApplicationId*>( &clientIterator.key() )->setProcessId( command.id().processId() );

                } else if( parser.hasFlag( "--abort" ) ) {

                    // tell existing client to die
                    ServerCommand abortCommand( command.id(), ServerCommand::CommandType::Abort );
                    existingClient->sendCommand( abortCommand );
                    _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Killed ), existingClient );

                    // tell new client it is denied too
                    sender->sendCommand( ServerCommand( command.id(), ServerCommand::CommandType::Denied ) );
                    _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Identify ), sender );
                    _register( command.id(), sender, true );

                } else {

                    // tell existing client to raise itself
                    ServerCommand raiseCommand( command.id(), ServerCommand::CommandType::Raise );
                    raiseCommand.setArguments( command.arguments() );
                    existingClient->sendCommand( raiseCommand );

                }

                return;
            }

            case ServerCommand::CommandType::Alive:
            {
                // client exist and is alive. Deny current
                _broadcast( ServerCommand( command.id(), ServerCommand::CommandType::Denied ) );
                return;

            }

            case ServerCommand::CommandType::Identify:
            {

                // identify the server
                sender->sendCommand( ServerCommand( id_, ServerCommand::CommandType::IdentifyServer ) );

                /*
                identification request. Loop over registered clients
                send the associated Identity to the sender
                */

                for( auto&& it=acceptedClients_.begin(); it!=acceptedClients_.end(); it++ )
                { sender->sendCommand( ServerCommand( it.key(), ServerCommand::CommandType::Identify ) ); }

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
        Debug::Throw( QStringLiteral("ApplicationManager::_newConnection.\n") );
        while( server_->hasPendingConnections() )
        {
            // create client from pending connection
            auto client = std::make_shared<Client>( this, server_->nextPendingConnection() );
            connect( client.get(), &Client::commandAvailable, this, QOverload<Server::ServerCommand>::of( &ApplicationManager::_redirect ) );
            connect( &client->socket(), &QAbstractSocket::disconnected, this, &ApplicationManager::_clientConnectionClosed );
            connectedClients_.append( client );
        }

    }

    //_____________________________________________________
    void ApplicationManager::_serverConnectionClosed()
    {
        Debug::Throw( QStringLiteral("ApplicationManager::_serverConnectionClosed - lost connection to server.\n") );
        serverInitialized_ = false;
        initialize();
    }

    //_____________________________________________________
    void ApplicationManager::_clientConnectionClosed()
    {

        Debug::Throw( QStringLiteral("ApplicationManager::_clientConnectionClosed - client has disconnected.\n") );

        // look for disconnected clients in client map
        for( auto&& iter = acceptedClients_.begin(); iter != acceptedClients_.end(); )
        {
            if( iter.value()->socket().state() == QAbstractSocket::UnconnectedState )
            {

                // broadcast client as dead and erase from list
                _broadcast( ServerCommand( iter.key(), ServerCommand::CommandType::Killed ), iter.value() );
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
            if( state_ == State::Dead ) return;

            // try initialize server
            if( !serverInitialized_ )
            {

                _initializeServer();
                _initializeClient();

            } else if( setState( State::Alive  ) ) {

                emit commandRecieved( ServerCommand( id_, ServerCommand::CommandType::Accepted ) );

            }

        } else {
            Debug::Throw() << "ApplicationManager::_error - unhandled error:" << client_->socket().errorString() << endl;
        }

        return;
    }

    //_____________________________________________________
    void ApplicationManager::_redirect( ServerCommand command )
    {

        Debug::Throw( QStringLiteral("Application::_redirect.\n") );

        _redirect( command, *std::find_if(  connectedClients_.begin(), connectedClients_.end(), Client::SameIdFTor( command.clientId() ) ) );
        return;
    }


    //_____________________________________________________
    void ApplicationManager::_process( ServerCommand command )
    {

        Debug::Throw() << "ApplicationManager::_process -"
            << " app:" << command.id().name()
            << " command: " << command.commandName()
            << endl;

        // check command id is valid
        if( !command.id().isValid() ) return;

        // check id match
        if( !( command.id() == id() ) ) return;

        switch( command.command() )
        {

            case ServerCommand::CommandType::Raise:
            if( state_ == State::Alive )
            {

                client_->sendCommand( ServerCommand( id_, ServerCommand::CommandType::Alive ) );
                emit commandRecieved( command );

            }
            break;

            case ServerCommand::CommandType::Denied:
            if( state_ == State::AwaitingReply )
            {
                timer_.stop();
                if( setState( State::Dead ) ) emit commandRecieved( command );
            }
            break;


            case ServerCommand::CommandType::Abort:
            if( state_ == State::Alive )
            {
                timer_.stop();
                if( setState( State::Dead ) ) emit commandRecieved( command );
            }
            break;

            case ServerCommand::CommandType::Accepted:
            if( state_ == State::AwaitingReply )
            {
                timer_.stop();
                if( setState( State::Alive ) ) emit commandRecieved( command );
            }
            break;

            default:
            {
                emit commandRecieved( command );
                break;
            }

        }

        return;

    }

    //__________________________________________________________________________
    bool ApplicationManager::_initializeServer()
    {

        Debug::Throw( QStringLiteral("ApplicationManager::_initializeServer.\n") );

        serverInitialized_ = true;

        server_.reset( new QTcpServer( this ) );
        connect( server_.get(), &QTcpServer::newConnection, this, &ApplicationManager::_newConnection );

        return server_->listen( host_, port_ );

    }

    //__________________________________________________________________________
    bool ApplicationManager::_initializeClient()
    {

        Debug::Throw() << "ApplicationManager::_initializeClient - connecting to host: " << host_.toString() << " port: " << port_ << endl;

        // connect client to port
        client_.reset( new Client( this ) );
        connect( &client_->socket(), QOverload<QAbstractSocket::SocketError>::of( &QAbstractSocket::error ), this, &ApplicationManager::_error );
        connect( &client_->socket(), &QAbstractSocket::connected, this, &ApplicationManager::_startTimer );
        connect( &client_->socket(), &QAbstractSocket::disconnected, this, &ApplicationManager::_serverConnectionClosed );
        connect( client_.get(), &Client::commandAvailable, this, &ApplicationManager::_process );
        client_->socket().connectToHost( host_, port_ );

        // emit initialization signal
        emit initialized();
        setState( State::AwaitingReply );

        // create request command
        ServerCommand command( id_, ServerCommand::CommandType::Request );

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

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
   \file    ApplicationManager.cpp
   \brief   ensures only one instance of an application is running
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <algorithm>


#include "ApplicationManager.h"
#include "Client.h"
#include "Debug.h"
#include "Util.h"
#include "XmlOptions.h"

using namespace std;
using namespace SERVER;


//_________________________________________
ApplicationManager::ApplicationManager( QObject* parent ):
  QObject( parent ),
  Counter( "ApplicationManager" ),
  host_( QHostAddress::LocalHost ),
  port_( 8082 ),
  server_( new QTcpServer( this ) ),
  server_initialized_( false ),
  client_( new Client( this, new QTcpSocket( this ) ) ),
  state_( AWAITING_REPLY )
{

  Debug::Throw( "ApplicationManager::ApplicationManager.\n" );
  setApplicationName( "GENERIC_APPLICATION" );

  connect( &_server(), SIGNAL( newConnection() ), SLOT( _newConnection() ) );

  // create new socket
  connect( &client().socket(), SIGNAL( error( QAbstractSocket::SocketError ) ), SLOT( _error( QAbstractSocket::SocketError ) ) );
  connect( &client().socket(), SIGNAL( connected() ), SLOT( _startTimer() ) );
  connect( &client().socket(), SIGNAL( disconnected() ), SLOT( _serverConnectionClosed() ) );
  connect( &client(), SIGNAL( commandAvailable( SERVER::ServerCommand ) ), SLOT( _process( SERVER::ServerCommand ) ) );

  if( !XmlOptions::get().find( "SERVER_HOST" ) )
  { XmlOptions::get().set( "SERVER_HOST", Option( QHostAddress( QHostAddress::LocalHost ).toString() ) ); }

  if( !XmlOptions::get().find( "SERVER_PORT" ) )
  { XmlOptions::get().set( "SERVER_PORT", Option( "8090" ), "default port" ); }

}

//_________________________________________
ApplicationManager::~ApplicationManager( void )
{
  Debug::Throw( "ApplicationManager::~ApplicationManager.\n" );
  delete client_;
  delete server_;
}

//_________________________________________
CommandLineParser ApplicationManager::commandLineParser( CommandLineArguments arguments, bool ignore_warnings )
{

  CommandLineParser out;
  out.registerFlag( "--replace", "replace existing application instance with new one" );
  out.registerFlag( "--no-server", "ignore server mode. Run new application instance" );
  out.registerFlag( "--abort", "exit existing instance" );
  out.registerOption( "--server-host", "string", "use specified host for server communication" );
  out.registerOption( "--server-port", "interger", "use specified port for server communication" );

  // these are additional flags recognized by Qt.
  // this may be system dependent, and vary from one Qt version to the other,
  // but is not very important. They are listed here only to avoid warnings from the application.
  out.registerOption( "-style", "string", "Qt widget style" );
  out.registerOption( "-graphicssystem", "string", "Qt drawing backend (raster|opengl)" );

  if( !arguments.isEmpty() )
  { out.parse( arguments, ignore_warnings ); }

  return out;

}

//_________________________________________
void ApplicationManager::usage( void )
{
  Debug::Throw(0) << "server mode options: " << endl;
  Debug::Throw(0) << "Server mode is used to avoid that multiple instances of the same application run at the same time. " << endl;
  Debug::Throw(0) << "Following options are used to control the running instance, or ignore this mode." << endl;
  commandLineParser().usage();
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
  id_ = ApplicationId( name, Util::user(), Util::env( "DISPLAY", "0.0" ).replace( ":", "" ) );
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
    // the application is then set to ALIVE
    if( state_ == AWAITING_REPLY && setState( ALIVE ) )
    { emit commandRecieved( ServerCommand( id_, ServerCommand::ACCEPTED ) ); }

  }

  return QObject::timerEvent( event );

}

//_____________________________________________________
Client* ApplicationManager::_register( const ApplicationId& id, Client* client, bool forced )
{
  Debug::Throw( "ApplicationManager::_register.\n" );

  if( forced ) {

    accepted_clients_[id] = client;
    return client;

  } else if( accepted_clients_.find( id ) == accepted_clients_.end() ) {

    accepted_clients_[id] = client;
    return client;

  } else {

    return accepted_clients_[id];

  }

}

//_____________________________________________________
void ApplicationManager::_redirect( ServerCommand command, Client* sender )
{

  Debug::Throw() << "ApplicationManager::_redirect -"
    << "  app:" << command.id().name()
    << " command: " << command.commandName()
    << endl;

  CommandLineParser parser( commandLineParser( command.arguments() ) );
  switch( command.command() )
  {

    case ServerCommand::UNLOCK:
    {

      // unlock request. Clear list of registered applications
      accepted_clients_.clear();
      return;

    }

    case ServerCommand::REQUEST:
    {

      // server request
      Client *existing_client = _register( command.id(), sender );

      if( sender == existing_client ) {

        // tell client it is accepted
        sender->sendCommand( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );

      } else if( parser.hasFlag( "--replace" ) ) {

        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendCommand( abort_command );
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );

        // tell new client it is accepted
        sender->sendCommand( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );
        _register( command.id(), sender, true );

      } else if( parser.hasFlag( "--abort" ) ) {

        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendCommand( abort_command );
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );

        // tell new client it is denied too
        sender->sendCommand( ServerCommand( command.id(), ServerCommand::DENIED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );
        _register( command.id(), sender, true );

      } else {

        // tell existing client to raise itself
        ServerCommand raise_command( command.id(), ServerCommand::RAISE );
        raise_command.setArguments( command.arguments() );
        existing_client->sendCommand( raise_command );

      }

      return;
    }

    case ServerCommand::ALIVE:
    {
      // client exist and is alive. Deny current
      _broadcast( ServerCommand( command.id(), ServerCommand::DENIED ) );
      return;

    }

    case ServerCommand::IDENTIFY:
    {
      /*
      identification request. Loop over registered clients
      send the associated Identity to the sender
      */

      for( ClientMap::iterator it=accepted_clients_.begin(); it!=accepted_clients_.end(); it++ )
      { sender->sendCommand( ServerCommand( it->first, ServerCommand::IDENTIFY ) ); }

      // identify the server
      sender->sendCommand( ServerCommand( id_, ServerCommand::IDENTIFY_SERVER ) );
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
  connect( client, SIGNAL( commandAvailable( SERVER::ServerCommand ) ), SLOT( _redirect( SERVER::ServerCommand ) ) );
  connect( &client->socket(), SIGNAL( disconnected() ), SLOT( _clientConnectionClosed() ) );
  _connectedClients().push_back( client );

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
    while( ( iter = find_if(  _acceptedClients().begin(), _acceptedClients().end(), SameStateFTor( QAbstractSocket::UnconnectedState ) )  ) != _acceptedClients().end() )
    {

      // broadcast client as dead
      _broadcast( ServerCommand( iter->first, ServerCommand::KILLED ), iter->second );

      // erase from map of accepted clients
      _acceptedClients().erase( iter );

    }
  }

  // look for disconnected clients in connected clients list
  {
    ClientList::iterator iter;
    while( ( iter = find_if( _connectedClients().begin(), _connectedClients().end(), SameStateFTor( QAbstractSocket::UnconnectedState ) ) ) != _connectedClients().end() )
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
    if( state_ == DEAD ) return;

    // try initialize server
    if( !_serverInitialized() )
    {

      _initializeServer();
      _initializeClient();

    } else if( setState( ALIVE  ) ) {

      emit commandRecieved( ServerCommand( id_, ServerCommand::ACCEPTED ) );

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

  ClientList::iterator iter( find_if(  _connectedClients().begin(), _connectedClients().end(), Client::SameIdFTor( command.clientId() ) ) );
  assert( iter != _connectedClients().end() );
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

  assert( client().id() == command.clientId() );

  // check command id is valid
  if( !command.id().isValid() ) return;

  // check id match
  if( !( command.id() == id() ) ) return;

  switch( command.command() )
  {

    case ServerCommand::RAISE:
    if( state_ == ALIVE )
    {

      client().sendCommand( ServerCommand( id_, ServerCommand::ALIVE ) );
      emit commandRecieved( command );
      return;

    } else break;

    case ServerCommand::DENIED:
    if( state_ == AWAITING_REPLY )
    {
      timer_.stop();
      if( setState( DEAD ) ) emit commandRecieved( command );
      return;
    } else break;


    case ServerCommand::ABORT:
    if( state_ == ALIVE )
    {
      timer_.stop();
      if( setState( DEAD ) ) emit commandRecieved( command );
      return;
    } else break;

    case ServerCommand::ACCEPTED:
    if( state_ == AWAITING_REPLY )
    {
      timer_.stop();
      if( setState( ALIVE ) ) emit commandRecieved( command );
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
  setState( AWAITING_REPLY );

  // create request command
  ServerCommand command( id(), ServerCommand::REQUEST );

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
  int timeout_delay( XmlOptions::get().find( "SERVER_TIMEOUT_DELAY" ) ? XmlOptions::get().get<int>( "SERVER_TIMEOUT_DELAY" ) : 2000 );
  timer_.start( timeout_delay, this );
}

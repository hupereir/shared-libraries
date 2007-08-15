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
   \file    ApplicationManager.cc
   \brief   ensures only one instance of an application is running
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <sstream>

#include "ApplicationManager.h"
#include "Client.h"
#include "Debug.h"
#include "Util.h"
#include "Server.h"
#include "XmlOptions.h"

using namespace std;
using namespace SERVER;

static const int debug_level(1);

//_________________________________________
ApplicationManager::ApplicationManager( QObject* parent ):
  QObject( parent ),
  Counter( "ApplicationManager" ),
  server_( 0 ),
  client_( 0 ),
  state_( AWAITING_REPLY ),
  timer_( this )
{ 

  Debug::Throw( debug_level, "ApplicationManager::ApplicationManager.\n" );
  setApplicationName( "GENERIC_APPLICATION" ); 

  server_ = new Server( this, SERVER_PORT );
  server_->listen( QHostAddress::Any, SERVER_PORT );
  connect( server_, SIGNAL( newConnection() ), SLOT( _newConnection() ) );

  // create new socket
  QTcpSocket* socket = new QTcpSocket( this );
  client_ = new Client( this, socket );
  client().socket().connectToHost( "localhost", SERVER_PORT );    
  connect( &client().socket(), SIGNAL( error( QAbstractSocket::SocketError ) ), SLOT( _error( QAbstractSocket::SocketError ) ) );
  connect( &client().socket(), SIGNAL( disconnected() ), SLOT( _recreateServer() ) );
  connect( client_, SIGNAL( messageAvailable() ), SLOT( _process() ) );
  
}

//_________________________________________
ApplicationManager::~ApplicationManager( void )
{ Debug::Throw( debug_level, "ApplicationManager::~ApplicationManager.\n" ); }

//_________________________________________
void ApplicationManager::usage( void )
{
  cout << "  --replace\t\t replace existing instance with new one." << endl;
  cout << "  --no-server\t\t ignore server mode. runs new application instance." << endl;
  cout << "  --abort\t\t exit existing instance." <<  endl;
}

//_____________________________________________________
void ApplicationManager::init( ArgList args, bool forced )
{

  Debug::Throw( debug_level, "ApplicationManager::init.\n" );

  
  if( forced ) 
  {
    server_->listen( QHostAddress::Any, SERVER_PORT );
    client().socket().abort();
    client().socket().connectToHost( "localhost", SERVER_PORT );    
  }
  
  // emit initialization signal
  emit initialized();
  setState( AWAITING_REPLY );
  
  // create request command
  ServerCommand command( id(), ServerCommand::REQUEST );
  
  // add command line arguments if any
  command.setArguments( args );
  Debug::Throw( debug_level ) << "ApplicationManager::init - " << command << endl;
  
  // send request command
  client_ ->sendMessage( command );
  
  // run timeout timer to force state ALIVE if no reply comes
  connect( &timer_, SIGNAL( timeout() ), SLOT( _replyTimeOut() ) );
  
  // time out delay (for existing server to reply)
  int timeout_delay( XmlOptions::get().find( "SERVER_TIMEOUT_DELAY" ) ? XmlOptions::get().get<int>( "SERVER_TIMEOUT_DELAY" ) : 2000 ); 
  timer_.setSingleShot( true );
  timer_.start( timeout_delay );
  
  Debug::Throw( debug_level, "ApplicationManager::init. done.\n" );
  
}  

//_____________________________________________________
void ApplicationManager::setApplicationName( const string& name )
{ 
  Debug::Throw( debug_level, "ApplicationManager::setApplicationName.\n" );
  id_ = ApplicationId( name, Util::user(), Util::env( "DISPLAY", "0.0" ) );
}


//_____________________________________________________
Client* ApplicationManager::_register( const ApplicationId& id, Client* client, bool forced )
{
  Debug::Throw( debug_level, "ApplicationManager::_register.\n" );
  
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
void ApplicationManager::_redirect( const std::string& message, Client* sender )
{
  
  Debug::Throw( debug_level ) << "Application::_redirect - message: " << message << endl;

  // parse message
  istringstream in( message );
  while( (in.rdstate() & ios::failbit ) == 0 ) {
    
    ServerCommand command;
    in >> command;
    if( !command.id().isValid() ) continue;
    
    if( command.command() == ServerCommand::UNLOCK ) 
    {
      
      // unlock request. Clear list of registered applications
      accepted_clients_.clear();
      continue;
      
    } else if( command.command() == ServerCommand::REQUEST ) {
      
      // server request 
      Client *existing_client = _register( command.id(), sender );
      
      if( sender == existing_client ) { 

        // tell client it is accepted
        sender->sendMessage( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );
                
      } else if( command.args().find( "--replace" ) ) {
         
        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendMessage( abort_command );     
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );
        
        // tell new client it is accepted   
        sender->sendMessage( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );
        _register( command.id(), sender, true );
        
      } else if( command.args().find( "--abort" ) ) {

        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendMessage( abort_command );     
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );
        
        // tell new client it is denied too   
        sender->sendMessage( ServerCommand( command.id(), ServerCommand::DENIED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), sender );
        _register( command.id(), sender, true );
      
      } else {
        
        // tell existing client to raise itself
        ServerCommand raise_command( command.id(), ServerCommand::RAISE );
        raise_command.setArguments( command.args() );
        existing_client->sendMessage( raise_command ); 
               
      }
      
      continue;
            
    } else if( command.command() == ServerCommand::ALIVE ) 
    {
 
      // client exist and is alive. Deny current
      _broadcast( ServerCommand( command.id(), ServerCommand::DENIED ) );
    
    } else if( command.command() == ServerCommand::IDENTIFY ) {
      
      /* 
      identification request. Loop over registered clients
      send the associated Identity to the sender
      */
      
      for( ClientMap::iterator it=accepted_clients_.begin(); it!=accepted_clients_.end(); it++ )
      { sender->sendMessage( ServerCommand( it->first, ServerCommand::IDENTIFY ) ); }
      
      // identify the server
      sender->sendMessage( ServerCommand( id_, ServerCommand::IDENTIFY_SERVER ) );
      
      continue;
    }
    
    // redirect unrecognized message to all clients but the sender
    _broadcast( command, sender );
    
  }

}

//_____________________________________________________
void ApplicationManager::_broadcast( const string& message, Client* sender )
{
  Debug::Throw( debug_level ) << "ApplicationManager::_Broadcast - message: " << message << endl;
  
  for( ClientList::iterator iter = _connectedClients().begin(); iter != _connectedClients().end(); iter++ )
  { if( (*iter) != sender ) (*iter)->sendMessage( message ); }
  
}

//_____________________________________________________
void ApplicationManager::_newConnection()
{
  Debug::Throw( debug_level, "ApplicationManager::_newConnection.\n" );
  
  // check pending connection
  if( !server_->hasPendingConnections() ) return;
  
  // create client from pending connection 
  Client *client( new Client( this, server_->nextPendingConnection() ) );
  connect( client, SIGNAL( messageAvailable() ), SLOT( _redirect() ) );
  connect( &client->socket(), SIGNAL( disconnected() ), SLOT( _connectionClosed() ) );
  _connectedClients().push_back( client );
  
}

//_____________________________________________________
void ApplicationManager::_connectionClosed( void )
{
  Debug::Throw( debug_level, "ApplicationManager::_connectionClosed.\n" );
  
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
    { _connectedClients().erase( iter ); }
    
    return;
  } 
  
}
  
//_____________________________________________________
void ApplicationManager::_error( QAbstractSocket::SocketError error )
{
  Debug::Throw( debug_level ) << "ApplicationManager::_error - error=" << error << endl;

  // when an error occur and state is not dead, state is forced alive
  if( state_ != DEAD ) setState( ALIVE );
  
  return;
}

//_____________________________________________________
void ApplicationManager::_recreateServer( void )
{
  Debug::Throw( debug_level, "ApplicationManager::_recreateServer.\n" );
  init( ArgList(), true );
}

//_____________________________________________________
void ApplicationManager::_replyTimeOut( void )
{
  Debug::Throw( debug_level, "ApplicationManager::_replyTimeOut.\n" );
  if( state_ == AWAITING_REPLY ) setState( ALIVE );
}

//_____________________________________________________
void ApplicationManager::_redirect( void )
{
  
  Debug::Throw( debug_level, "Application::_redirect.\n" );

  ClientList::iterator iter;
  while( ( iter = find_if(  _connectedClients().begin(), _connectedClients().end(), HasMessageFTor() ) ) != _connectedClients().end() )
  { 
    if( !(*iter)->hasMessage() ) continue;
    string message( (*iter)->message() );
    (*iter)->reset();
    _redirect( message, *iter );
  }

  return;
}


//_____________________________________________________
void ApplicationManager::_process( void )
{
  
  Debug::Throw( debug_level, "Application::_process.\n" );
  
  Exception::check( client().hasMessage(), DESCRIPTION( "no message available" ) );
  
  // retrieve message and reset
  string message( client().message() );
  client().reset();
  
  istringstream in( message );
  while( (in.rdstate() & ios::failbit ) == 0 ) 
  {
    
    ServerCommand command;
    in >> command;
    
    // check command id is valid
    if( !command.id().isValid() ) continue;
    
    // check id match
    if( !( command.id() == id() ) ) continue; 
    
    // if client is alive and message is matching server_request, send server_exist
    if( state_ == ALIVE && ( command.command() == ServerCommand::RAISE ) ) 
    {
      
      client().sendMessage( ServerCommand( id_, ServerCommand::ALIVE ) );
      emit serverRequest( command.args() );
      continue;
    
    }
  
    // if client awaits reply and connection is refused, 
    if( state_ == AWAITING_REPLY && command.command() == ServerCommand::DENIED ) 
    {
      setState( DEAD );
      timer_.stop();
      continue;
    }
  
    // client is alive and recieved an ABORT command.
    if( state_ == ALIVE && command.command() == ServerCommand::ABORT ) 
    {
      setState( DEAD );
      timer_.stop();
      continue;
    }
   
    // if client awaits reply and connection is accepted, stay alive
    if( state_ == AWAITING_REPLY && command.command() == ServerCommand::ACCEPTED ) 
    {
      setState( ALIVE );
      timer_.stop();
      continue;
    }
 
  }
  
  return;
  
}

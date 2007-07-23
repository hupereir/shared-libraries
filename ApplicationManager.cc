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
#include "SERVER.h"
#include "Server.h"
#include "XmlOptions.h"

using namespace std;
using namespace SERVER;

//_________________________________________
ApplicationManager::ApplicationManager( QObject* parent ):
  QObject( parent ),
  Counter( "ApplicationManager" ),
  server_( 0 ),
  client_( 0 ),
  state_( AWAITING_REPLY ),
  timer_( this )
{ 

  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::ApplicationManager.\n" );
  setApplicationName( "GENERIC_APPLICATION" ); 

}

//_________________________________________
ApplicationManager::~ApplicationManager( void )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::~ApplicationManager.\n" );
  
  // close all connected clients
  for( list<Client*>::iterator iter = connected_clients_.begin(); iter != connected_clients_.end(); iter++ )
  {
    (*iter)->socket().abort();
    (*iter)->socket().close();
  }
  
  // delete server
  if( server_ ) delete server_;

}

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

  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::init.\n" );

  //! initialize server
  if( server_ && forced ) {
    delete server_;
    server_ = 0;
  }
  
  if( !server_ ) {
    server_ = new Server( this, 4242 );
    connect( server_, SIGNAL( newConnection() ), this, SLOT( _newConnection() ) );
  }
  
  //! initialize client 
  if( client_ && forced ) {
    delete client_;
    client_ = 0;
  }
  
  // create the client
  if( !client_ ) {
    
    // create new socket
    QTcpSocket* socket = new QTcpSocket( this );
    socket->connectToHost( "localhost", SERVER_PORT );    
    
    client_ = new Client( socket );
    connect( socket, SIGNAL( error( QAbstractSocket::SocketError )), this, SLOT( _error( QAbstractSocket::SocketError ) ) );
    connect( socket, SIGNAL( disconnected() ), this, SLOT( _recreateServer() ) );
    connect( client_, SIGNAL( messageAvailable( SERVER::Client*, const std::string& ) ), this, SLOT( _processMessage( SERVER::Client*, const std::string& ) ) );
  }
  
  // emit initialization signal
  emit initialized();
  setState( AWAITING_REPLY );
  
  // create request command
  ServerCommand command( id(), ServerCommand::REQUEST );
  
  // add command line arguments if any
  command.setArguments( args );
  Debug::Throw( DEBUG_LEVEL ) << "ApplicationManager::init - " << command << endl;
  
  // send request command
  client_ ->sendMessage( command );
  
  // run timeout timer to force state ALIVE if no reply comes
  connect( &timer_, SIGNAL( timeout() ), this, SLOT( _replyTimeOut() ) );
  
  // time out delay (for existing server to reply)
  int timeout_delay( XmlOptions::get().find( "SERVER_TIMEOUT_DELAY" ) ? XmlOptions::get().get<int>( "SERVER_TIMEOUT_DELAY" ) : 2000 ); 
  timer_.setSingleShot( true );
  timer_.start( timeout_delay );
  
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::init. done.\n" );
  
}  

//_____________________________________________________
void ApplicationManager::setApplicationName( const string& name )
{ 
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::setApplicationName.\n" );
  id_ = ApplicationId( name, Util::user(), Util::env( "DISPLAY", "0.0" ) );
}

//_____________________________________________________
void ApplicationManager::_newConnection()
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_newConnection.\n" );
  
  // check pending connection
  if( !server_->hasPendingConnections() ) return;
  
  // create client from pending connection 
  Client *client( new Client( server_->nextPendingConnection() ) );
  connect( client, SIGNAL( messageAvailable( SERVER::Client*, const std::string& ) ), this, SLOT( _redirectMessage( SERVER::Client*, const std::string& ) ) );
  connect( client, SIGNAL( disconnected( SERVER::Client* ) ), this, SLOT( _connectionClosed( SERVER::Client* ) ) );
  connected_clients_.push_back( client );
  
}

//_____________________________________________________
void ApplicationManager::_connectionClosed( Client* client )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_connectionClosed.\n" );
  
  // look for client in accepted clients map
  ClientMap tmp_map;
  for( ClientMap::iterator it = accepted_clients_.begin(); it != accepted_clients_.end(); it++ )
  if( it->second != client ) tmp_map.insert( *it );
  else _broadcast( ServerCommand( it->first, ServerCommand::KILLED ), client );
  
  accepted_clients_ = tmp_map;

  // look for client in list; remove if found
  connected_clients_.remove( client );
  delete client;
  
  return;

}

//_____________________________________________________
void ApplicationManager::_error( QAbstractSocket::SocketError error )
{
  Debug::Throw( DEBUG_LEVEL ) << "ApplicationManager::_error - error=" << error << endl;

  // when an error occur and state is not dead, state is forced alive
  if( state_ != DEAD ) setState( ALIVE );
  
  return;
}

//_____________________________________________________
void ApplicationManager::_recreateServer( void )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_recreateServer.\n" );
  init( ArgList(), true );
}

//_____________________________________________________
void ApplicationManager::_replyTimeOut( void )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_replyTimeOut.\n" );
  if( state_ == AWAITING_REPLY ) setState( ALIVE );
}

//_____________________________________________________
void ApplicationManager::_redirectMessage( Client* client, const string& message )
{
  
  Debug::Throw( DEBUG_LEVEL ) << "Application::_redirectMessage - message = " << message << endl;

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
      
    } else if( client && command.command() == ServerCommand::REQUEST ) {
      
      // server request 
      Client *existing_client = _register( command.id(), client );
      
      if( client == existing_client ) { 

        // tell client it is accepted
        client->sendMessage( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), client );
                
      } else if( command.args().find( "--replace" ) ) {
         
        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendMessage( abort_command );     
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );
        
        // tell new client it is accepted   
        client->sendMessage( ServerCommand( command.id(), ServerCommand::ACCEPTED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), client );
        _register( command.id(), client, true );
        
      } else if( command.args().find( "--abort" ) ) {

        // tell existing client to die
        ServerCommand abort_command( command.id(), ServerCommand::ABORT );
        existing_client->sendMessage( abort_command );     
        _broadcast( ServerCommand( command.id(), ServerCommand::KILLED ), existing_client );
        
        // tell new client it is denied too   
        client->sendMessage( ServerCommand( command.id(), ServerCommand::DENIED ) );
        _broadcast( ServerCommand( command.id(), ServerCommand::IDENTIFY ), client );
        _register( command.id(), client, true );
      
      } else {
        
        // tell existing client to raise itself
        ServerCommand raise_command( command.id(), ServerCommand::RAISE );
        raise_command.setArguments( command.args() );
        existing_client->sendMessage( raise_command ); 
               
      }
      
      continue;
            
    } else if( client && command.command() == ServerCommand::ALIVE ) 
    {
 
      // client exist and is alive. Deny current
      _broadcast( ServerCommand( command.id(), ServerCommand::DENIED ) );
    
    } else if( command.command() == ServerCommand::IDENTIFY ) {
      /* 
      identification request. Loop over registered clients
      send the associated Identity to the sender
      */
      
      for( ClientMap::iterator it=accepted_clients_.begin(); it!=accepted_clients_.end(); it++ )
      { client->sendMessage( ServerCommand( it->first, ServerCommand::IDENTIFY ) ); }
      
      // identify the server
      client->sendMessage( ServerCommand( id_, ServerCommand::IDENTIFY_SERVER ) );
      
      continue;
    }
    
    // redirect unrecognized message to all clients but the sender
    _broadcast( command, client );
    
  }

}

//_____________________________________________________
void ApplicationManager::_processMessage( Client* client, const string& message )
{
  
  Debug::Throw( DEBUG_LEVEL ) << "Application::_processMessage - message = " << message << " state=" << state_ << endl;
  
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
      
      client_->sendMessage( ServerCommand( id_, ServerCommand::ALIVE ) );
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

//_____________________________________________________
Client* ApplicationManager::_register( const ApplicationId& id, Client* client, bool forced )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_register.\n" );
  
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
void ApplicationManager::_broadcast( const string& message, Client* sender )
{
  Debug::Throw( DEBUG_LEVEL, "ApplicationManager::_Broadcast.\n" );
  
  for( list< Client* >::iterator iter = connected_clients_.begin(); iter != connected_clients_.end(); iter++ )
  { if( (*iter) != sender ) (*iter)->sendMessage( message ); }
  
}

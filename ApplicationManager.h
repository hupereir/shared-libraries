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

#ifndef ApplicationManager_h
#define ApplicationManager_h

/*!
   \file    ApplicationManager.h
   \brief   ensures only one instance of an application is running
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QObject>
#include <QTimer>
#include <QTcpSocket>

#include <iostream>
#include <list>
#include <map>
#include <string>

#include "ArgList.h"
#include "Client.h"
#include "Counter.h"
#include "Exception.h"
#include "Server.h"
#include "ServerCommand.h"

namespace SERVER
{

  //! ensures only one instance of an application is running
  class ApplicationManager: public QObject, public Counter
  {
  
    //! Qt meta object macro
    Q_OBJECT
  
    public:
    
    //! constructor
    ApplicationManager( QObject* parent );
    
    //! destructor
    virtual ~ApplicationManager( void );
      
    //! initialize client/server
    virtual void init( ArgList args, bool forced = false );
     
    //! application name
    virtual void setApplicationName( const std::string& name );
  
    //! comamnd line usage
    static void usage( void );
      
    //! application state enumeration
    enum State {
      
      //! application manager is uninitialized
      UNKNOWN,          
          
      //! application manager awaits signal from server
      AWAITING_REPLY,   
          
      //! application manager got no reply, is kept alive
      ALIVE,            
          
      //! application manager got a reply, must die
      DEAD              
    };
    
    //! changes application state, emit signal if changed
    virtual void setState( const State& state )
    {
      if( state == state_ ) return;
      emit stateChanged( (state_ = state ) ); 
    }
    
    //! retrieves reference to "this" client
    virtual Client& client( void )
    { 
      Exception::checkPointer( client_, DESCRIPTION( "client_ not initialized" ) );
      return *client_;
    }
    
    //! retrieve Application ID
    virtual const ApplicationId& id( void ) const
    { return id_; }
    
    signals:
    
    //! emitted when manager state is changed
    void stateChanged( SERVER::ApplicationManager::State state );
    
    //! emitted when manager is ALIVE and request is recieved
    void serverRequest( const ArgList& args );
   
    //! emitted when the server is (re)initialized
    void initialized( void );
      
    protected:
         
    //! pair of application id and client
    typedef std::pair< ApplicationId, Client* > ClientPair;
    
    //! map of clients
    typedef std::map< ApplicationId, Client* > ClientMap;
    
    //! list of clients
    typedef std::list< Client* > ClientList;

    //! used to retrieve clients for a given state
    class SameStateFTor
    {
      public:
      
      //! constructor
      SameStateFTor( QAbstractSocket::SocketState state ):
        state_( state )
        {}
        
      //! predicate
      bool operator() ( const Client* client ) const
      { return client->socket().state() == state_; }
      
      //! predicate
      bool operator() ( const ClientPair& pair ) const
      { return pair.second->socket().state() == state_; }
      
      private:
      
      //! prediction
      QAbstractSocket::SocketState state_;
        
    };
     
    //! used to retrieve clients with available messages
    class HasMessageFTor
    {
      
      public:
      
      //! predicate
      bool operator() ( const Client* client ) const
      { return client->hasMessage(); }
      
    };
    
    //! map of accepted clients
    ClientMap& _acceptedClients( void ) 
    { return accepted_clients_; }
    
    //! list of connected clients
    ClientList& _connectedClients( void )
    { return connected_clients_; }
    
    /*! \brief register a client, returns true if application is new.
      if forced is set to true, the old cliend, if any, is replaced
    */
    virtual Client* _register( const ApplicationId& id, Client* client, bool forced = false );

    //! redirect message 
    virtual void _redirect( const std::string& message, Client* sender );
    
    //! broadcast a message to all registered clients but the sender (if valid)
    virtual void _broadcast( const std::string& message, Client* sender = 0 );

    protected slots:
    
    //! a new connection is granted
    virtual void _newConnection( void );
        
    //! a connection was closed
    virtual void _connectionClosed( void );
    
    //! client recieves errors
    virtual void _error( QAbstractSocket::SocketError );
    
    //! a connection is dead
    virtual void _recreateServer( void );
    
    //! no reply came within delay
    void _replyTimeOut( void );
    
    //! redistribute message when a connected client sends one
    virtual void _redirect( void );
        
    //! reads signal from server
    void _process( void );
  
    private:
    
    //! Server 
    Server* server_;
    
    //! Client
    Client* client_;
    
    //! list of connected clients
    ClientList connected_clients_;
  
    //! maps accepted clients and amount of request
    ClientMap accepted_clients_;
  
    //! application name
    ApplicationId id_;
    
    //! manager status
    State state_;
  
    //! QTimer for reply timeout
    QTimer timer_;
  
  };
};

#endif

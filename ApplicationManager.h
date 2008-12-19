#ifndef ApplicationManager_h
#define ApplicationManager_h

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
   \file    ApplicationManager.h
   \brief   ensures only one instance of an application is running
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <assert.h>
#include <QBasicTimer>
#include <QObject>
#include <QTcpServer>
#include <QTimerEvent>

#include <iostream>
#include <list>
#include <map>
#include <string>

#include <QHostAddress>

#include "ArgList.h"
#include "Client.h"
#include "Counter.h"
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
     
    //! application name
    virtual void setApplicationName( const QString& name );
  
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
    
    //! reference to "this" client
    virtual Client& client( void ) const
    { 
      assert( client_ );
      return *client_;
    }
    
    //! retrieve Application ID
    virtual const ApplicationId& id( void ) const
    { return id_; }
    
    public slots:
    
    //! (re)initialize server/client connections
    virtual void initialize( ArgList args = ArgList() );
    
    signals:
    
    //! emitted when manager state is changed
    void stateChanged( SERVER::ApplicationManager::State state );
    
    //! emitted when manager is ALIVE and request is recieved
    void serverRequest( const ArgList& args );
   
    //! emitted when the server is (re)initialized
    void initialized( void );
      
    protected:
         
    //! timer event
    virtual void timerEvent( QTimerEvent* );
    
    //! reference to server
    virtual QTcpServer& _server() const
    { 
      assert( server_ );
      return *server_;
    }
    
    //! pair of application id and client
    typedef std::pair< ApplicationId, Client* > ClientPair;
    
    //! map of clients
    typedef std::map< ApplicationId, Client* > ClientMap;
    
    //! list of clients
    typedef std::list< Client* > ClientList;

    //! used to retrieve clients for a given state
    class SameStateFTor: public Client::SameStateFTor
    {
      public:
      
      //! constructor
      SameStateFTor( QAbstractSocket::SocketState state ):
        Client::SameStateFTor( state )
        {}
        
      //! destructor
      virtual ~SameStateFTor( void )
      {}
        
      //! predicate
      bool operator() ( const ClientPair& pair ) const
      { return Client::SameStateFTor::operator() (pair.second); }
              
      //! predicate
      bool operator() ( const Client* client ) const
      { return Client::SameStateFTor::operator() (client); }

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
    virtual void _redirect( QString message, Client* sender );
    
    //! broadcast a message to all registered clients but the sender (if valid)
    virtual void _broadcast( QString message, Client* sender = 0 );

    protected slots:
    
    //! a new connection is granted
    virtual void _newConnection( void );
        
    //! a connection was closed
    virtual void _serverConnectionClosed( void );
    
    //! a connection was closed
    virtual void _clientConnectionClosed( void );

    //! client recieves errors
    virtual void _error( QAbstractSocket::SocketError );
    
    //! redistribute message when a connected client sends one
    virtual void _redirect( void );
        
    //! reads signal from server
    void _process( void );
  
    private:
    
    //! host
    const QHostAddress& _host( void ) const
    { return host_; }
    
    //! host
    void _setHost( const QHostAddress& host ) 
    { host_ = host; }
    
    //! port
    const unsigned int& _port( void ) const
    { return port_; }
    
    //! port
    void _setPort( const unsigned int& port ) 
    { port_ = port; }
    
    //! arguments
    const ArgList& _arguments( void ) const
    { return arguments_; }
    
    //! arguments
    void _setArguments( const ArgList& arguments )
    { arguments_ = arguments; }

    // initialize client
    bool _initializeClient( void );
    
    //! initialize server
    bool _initializeServer( void );
    
    //! server initialization
    bool _serverInitialized( void ) const
    { return server_initialized_; }
    
    //! server initialization
    void _setServerInitialized( bool value ) 
    { server_initialized_ = value; }
      
    // host
    QHostAddress host_;
    
    // port
    unsigned int port_;
    
    // arguments
    ArgList arguments_;
    
    //! Server 
    QTcpServer* server_;
    
    //! true if initializeServer was called
    bool server_initialized_;
    
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
  
    //! reply timeout
    QBasicTimer timer_;
  
  };
};

#endif

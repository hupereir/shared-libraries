#ifndef Client_h
#define Client_h

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
   \file    Client.h
   \brief   Interprocess communication client
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QTcpSocket>
#include <string>
#include <list>

#include "Counter.h"
#include "MessageBuffer.h"
#include "ServerCommand.h"

namespace SERVER
{
  
  //! interprocess communication client
  class Client : public QObject, public Counter
  {
  
    //! Qt meta object macro
    Q_OBJECT
    
    public:
    
    //! client list
    typedef std::list< Client* > List;
      
    //! constructor
    Client( QObject* parent, QTcpSocket* socket );
  
    //! destructor
    virtual ~Client( void );

    //! id
    unsigned int id( void ) const
    { return id_; }
    
    //! associated socket
    QTcpSocket& socket()
    { return *socket_; }

    //! associated socket
    virtual const QTcpSocket& socket() const
    { return *socket_; }
    
    /*! returns true if message could be sent */
    bool sendCommand( const ServerCommand& );
    
    //! used to retrieve clients for a given state
    class SameStateFTor
    {
      public:
      
      //! constructor
      SameStateFTor( QAbstractSocket::SocketState state ):
        state_( state )
        {}
        
      //! destructor
      virtual ~SameStateFTor( void )
      {}
      
      //! predicate
      virtual bool operator() ( const Client* client ) const
      { return client->socket().state() == state_; }
            
      private:
      
      //! prediction
      QAbstractSocket::SocketState state_;
        
    };
     
    //! used to retrieve client matching id
    class SameIdFTor
    {
      
      public:
      
      //! constructor
      SameIdFTor( unsigned int id ):
        id_( id )
        {}
      
      //! destructor
      virtual ~SameIdFTor( void )
      {}
      
      //! predicate
      virtual bool operator() ( const Client* client ) const
      { return client->id() == id_; }
        
      protected:
      
      //! prediction
      unsigned int id_;
      
    };
    
    signals:
    
    //! emitted when a message is available
    virtual void commandAvailable( SERVER::ServerCommand );
     
    protected:
    
    //! buffer
    MessageBuffer& _messageBuffer( void )
    { return buffer_; }
    
    protected slots:
    
    //! reads messages
    virtual bool _readMessage( void );
    
    private slots:
    
    //! send all commands
    virtual void _sendCommands( void );
    
    private:    
    
    //! client counter
    static unsigned int& _counter( void );
    
    //! client id
    unsigned int id_;
    
    //! parent socket
    QTcpSocket* socket_;
    
    //! messages
    typedef std::list< ServerCommand > CommandList;
   
    //! commands
    CommandList commands_;
    
    //! buffer
    MessageBuffer buffer_;
    
  };
  
};

#endif

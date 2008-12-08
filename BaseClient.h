#ifndef BaseClient_h
#define BaseClient_h

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
   \file    BaseClient.h
   \brief   Interprocess communication client
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QTcpSocket>
#include <string>
#include <list>

#include "Counter.h"

namespace SERVER
{
  
  //! interprocess communication client
  class BaseClient : public QObject, public Counter
  {
  
    //! Qt meta object macro
    Q_OBJECT
    
    public:
    
    //! client list
    typedef std::list< BaseClient* > List;
      
    //! constructor
    BaseClient( QObject* parent, QTcpSocket* socket );
  
    //! destructor
    virtual ~BaseClient( void );

    //! associated socket
    QTcpSocket& socket()
    { return *socket_; }

    //! associated socket
    virtual const QTcpSocket& socket() const
    { return *socket_; }
    
    /*! returns true if message could be sent */
    virtual bool sendMessage( const QString& );
    
    //! true if message is available
    virtual const bool& hasMessage( void ) const
    { return has_message_; }
    
    //! current message
    virtual const QString& message( void ) const
    { return message_; }
    
    //! reset
    virtual void reset( void );
    
    //! used to retrieve clients for a given state
    class SameStateFTor
    {
      public:
      
      //! constructor
      SameStateFTor( QAbstractSocket::SocketState state ):
        state_( state )
        {}
        
      //! predicate
      virtual bool operator() ( const BaseClient* client ) const
      { return client->socket().state() == state_; }
            
      private:
      
      //! prediction
      QAbstractSocket::SocketState state_;
        
    };
     
    //! used to retrieve clients with available messages
    class HasMessageFTor
    {
      
      public:
      
      //! predicate
      bool operator() ( const BaseClient* client ) const
      { return client->hasMessage(); }
      
    };

    signals:
    
    //! emitted when a message is available
    virtual void messageAvailable();
        
    protected slots:
    
    //! reads messages
    virtual void _readMessage( void );
    
    private slots:
    
    //! send all commands
    virtual void _sendMessages( void );
    
    private:    
    
    //! parent socket
    QTcpSocket* socket_;
    
    //! true if has message available
    bool has_message_;
    
    //! messages
    typedef std::list< QString > MessageList;
    MessageList messages_;
    
    //! current message
    QString message_;
    
  };
};

#endif

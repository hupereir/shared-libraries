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

    //! associated socket
    QTcpSocket& socket()
    { return *socket_; }

    //! associated socket
    const QTcpSocket& socket() const
    { return *socket_; }
    
    /*! returns true if message could be sent */
    bool sendCommand( const ServerCommand& );
    
    //! true if message is available
    const bool& hasMessage( void ) const
    { return has_message_; }
    
    //! current message
    const QString& message( void ) const
    { return message_; }
    
    //! reset
    void reset( void );
    
    signals:
    
    //! emitted when a message is available
    void messageAvailable();
     
    private slots:

    //! send messages
    void _sendCommands( void );
    
    //! reads messages
    void _readMessage( void );
    
    private:    
    
    //! parent socket
    QTcpSocket* socket_;
    
    //! true if has message available
    bool has_message_;
    
    //! commands
    ServerCommand::List commands_;
    
    //! current message
    QString message_;
    
  };
};

#endif

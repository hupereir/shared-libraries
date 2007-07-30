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

#ifndef Client_h
#define Client_h

#include <QTcpSocket>
#include <string>

#include "Counter.h"

namespace SERVER
{
  
  //! interprocess communication client
  class Client : public QObject, public Counter
  {
  
    //! Qt meta object macro
    Q_OBJECT
    
    public:
    
    //! constructor
    Client( QTcpSocket* parent );
  
    //! destructor
    virtual ~Client( void )
    { Debug::Throw( "Client::~Client.\n" ); }

    //! associated socket
    QTcpSocket& socket()
    { return *socket_; }
    
    //! send message
    void sendMessage( const std::string& message );
    
    signals:
    
    //! emitted when a message is available
    void messageAvailable( SERVER::Client*, const std::string& message );
    
    //! emitted when a client is closed
    void disconnected( SERVER::Client* );
     
    private slots:
    
    //! reads messages
    void _readMessage( void );
    
    //! close connection
    void _connectionClosed( void );
    
    private:
    
    //! parent socket
    QTcpSocket* socket_;
     
  };
};

#endif

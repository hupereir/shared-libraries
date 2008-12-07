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

#include <string>
#include <list>

#include "Counter.h"
#include "BaseClient.h"
#include "ServerCommand.h"

namespace SERVER
{
  
  //! interprocess communication client
  class Client : public BaseClient
  {
  
    public:
    
    //! client list
    typedef std::list< Client* > List;
      
    //! constructor
    Client( QObject* parent, QTcpSocket* socket ):
      BaseClient( parent, socket )
    {}
  
    //! destructor
    virtual ~Client( void )
    {}
    
    /*! returns true if message could be sent */
    bool sendCommand( const ServerCommand& command )
    { return sendMessage( QString(command) ); }
             
  };
  
};

#endif

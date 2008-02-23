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
   \file    Server.h
   \brief   interprocess communication server
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#ifndef Server_h
#define Server_h

#include <QTcpServer>
#include <string>

#include "Counter.h"

//! Server classes
/*!
  These classes allows interprocess communication using sockets, to keep
  track of how many applications of same type are running and possibly allows
  only one instance of each to run.
*/
namespace SERVER
{

  //! default server port
  enum { 
    SERVER_PORT = 4242, 
    TEST_PORT = 4243
  };
  
  //!  interprocess communication server
  class Server: public QTcpServer, public Counter
  {
  
    //! Qt meta object macro
    Q_OBJECT
  
    public:
    
    //! constructor
    Server( QObject* parent, quint16 port = SERVER::SERVER_PORT  );
    
    //! destructor
    virtual ~Server( void );
    
  };
};
#endif

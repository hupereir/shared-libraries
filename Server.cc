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
   \file    Server.cc
   \brief   interprocess communication server
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Debug.h"
#include "Server.h"

using namespace std;
using namespace SERVER;

static const int debug_level(1);

//__________________________________________________
Server::Server( QObject *parent, quint16 port ):
  QTcpServer( parent ),
  Counter( "Server" )
{ 
  Debug::Throw( debug_level ) << "Server::Server - port: " << port << endl;
}

//__________________________________________________
Server::~Server( void )
{ Debug::Throw( debug_level, "Server::~Server.\n" ); }


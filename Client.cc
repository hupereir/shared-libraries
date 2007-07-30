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
   \file    Client.cc
   \brief   Interprocess communication client
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QTextStream>
#include <sstream>

#include "Client.h"
#include "Debug.h"

using namespace std;
using namespace SERVER;

//_______________________________________________________
Client::Client( QTcpSocket* parent ):
  QObject( parent ),
  Counter( "Client" ),
  socket_( parent )
{
  Debug::Throw( "Client::Client.\n" );
  Exception::checkPointer( parent, DESCRIPTION( "invalid Tcp socket" ) );
  connect( socket_, SIGNAL( readyRead() ), SLOT( _readMessage() ) );
  connect( socket_, SIGNAL( disconnected() ), SLOT( _connectionClosed() ) );
}

//_______________________________________________________
void Client::sendMessage( const string& message )
{
  Debug::Throw() << "Client::sendMessage - " << message << endl;
  QTextStream os( &socket() );
  os << message.c_str() << endl;
}

//_______________________________________________________
void Client::_readMessage( void )
{
  Debug::Throw( "Client::_readMessage.\n" );
  ostringstream out;
  
  // read from the server
  while ( socket().canReadLine() ) {
    string line( (const char*) socket().readLine() );
    if( line.size() && line != "\n" )
    out << line;
  }

  emit messageAvailable( this, out.str() );  

}

//_______________________________________________________
void Client::_connectionClosed( void )
{
  Debug::Throw( "Client::_connectionClosed.\n" );
  emit disconnected( this );
  return;
}

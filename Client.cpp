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
   \file    Client.cpp
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

static const int debug_level(1);

//_______________________________________________________
Client::Client( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "Client" ),
  socket_( socket ),
  has_message_( false )
{
  Debug::Throw( debug_level, "Client::Client.\n" );
  Exception::checkPointer( socket, DESCRIPTION( "invalid Tcp socket" ) );
  connect( socket_, SIGNAL( readyRead() ), SLOT( _readMessage() ) );
}

//_______________________________________________________
Client::~Client( void )
{ Debug::Throw( debug_level, "Client::~Client.\n" ); }

//_______________________________________________________
bool Client::sendMessage( const string& message )
{
  Debug::Throw( debug_level ) << "Client::sendMessage - " << message << endl;
  
  if( !socket().state() ==  QAbstractSocket::ConnectedState ) return false;
  QTextStream os( &socket() );
  os << message.c_str() << endl;
  return true;
  
}


//_______________________________________________________
void Client::reset( void )
{
  Debug::Throw( debug_level, "Client::reset.\n" );
  has_message_ = false;
}


//_______________________________________________________
void Client::_readMessage( void )
{
  Debug::Throw(  debug_level, "Client::_readMessage.\n" );
  ostringstream out;
  
  // read from the server
  while ( socket().canReadLine() ) {
    string line( (const char*) socket().readLine() );
    if( line.size() && line != "\n" )
    out << line;
  }

  // store message and emit signal
  has_message_ = true;
  message_ = out.str();
  emit messageAvailable();  

}

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
#include <assert.h>

#include "Client.h"
#include "Debug.h"
#include "IOString.h"

using namespace std;
using namespace SERVER;


//_______________________________________________________
Client::Client( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "Client" ),
  socket_( socket ),
  has_message_( false )
{
  Debug::Throw( "Client::Client.\n" );
  assert( socket );
  connect( socket_, SIGNAL( connected() ), SLOT( _sendCommands() ) );
  connect( socket_, SIGNAL( readyRead() ), SLOT( _readMessage() ) );
}

//_______________________________________________________
Client::~Client( void )
{ Debug::Throw( "Client::~Client.\n" ); }

//_______________________________________________________
bool Client::sendCommand( const ServerCommand& command )
{
  
  Debug::Throw() << "Client::sendCommand - " << qPrintable( QString( command ) ) << endl;
  commands_.push_back( command );
  if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendCommands();
  return true;
  
}

//_______________________________________________________
void Client::reset( void )
{
  Debug::Throw( "Client::reset.\n" );
  has_message_ = false;
}

//_______________________________________________________
void Client::_sendCommands( void )
{

  Debug::Throw( "Client::_sendCommands.\n" );
  while( commands_.size() && socket().state() == QAbstractSocket::ConnectedState )
  {
    QString message( commands_.front() );
    QTextStream os( &socket() );
    os << message << endl;
    commands_.pop_front();
  }
  
}

//_______________________________________________________
void Client::_readMessage( void )
{

  Debug::Throw( "Client::_readMessage.\n" );

  // read everything from socket and store as message
  message_ = IOString( socket() );  

  // store message and emit signal
  has_message_ = true;
  emit messageAvailable();  

}

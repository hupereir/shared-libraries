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
   \file    BaseClient.cpp
   \brief   Interprocess communication client
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QTextStream>
#include <assert.h>

#include "BaseClient.h"
#include "Debug.h"

using namespace std;
using namespace SERVER;


//_______________________________________________________
BaseClient::BaseClient( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "BaseClient" ),
  socket_( socket ),
  has_message_( false )
{
  Debug::Throw( "BaseClient::BaseClient.\n" );
  assert( socket );
  connect( socket_, SIGNAL( connected() ), SLOT( _sendMessages() ) );
  connect( socket_, SIGNAL( readyRead() ), SLOT( _readMessage() ) );
}

//_______________________________________________________
BaseClient::~BaseClient( void )
{ Debug::Throw( "BaseClient::~BaseClient.\n" ); }

//_______________________________________________________
bool BaseClient::sendMessage( const QString& message )
{
  
  Debug::Throw() << "BaseClient::sendCommand - " << qPrintable( message ) << endl;
  messages_.push_back( message );
  if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendMessages();
  return true;
  
}

//_______________________________________________________
void BaseClient::reset( void )
{
  Debug::Throw( "BaseClient::reset.\n" );
  has_message_ = false;
}

//_______________________________________________________
void BaseClient::_sendMessages( void )
{

  Debug::Throw( "BaseClient::_sendMessages.\n" );
  while( messages_.size() && socket().state() == QAbstractSocket::ConnectedState )
  {
    QString message( messages_.front() );
    //QTextStream os( &socket(), QIODevice::WriteOnly );
    QTextStream os( &socket() );
    os << message << endl;
    messages_.pop_front();
  }
  
}

//_______________________________________________________
void BaseClient::_readMessage( void )
{

  Debug::Throw( "BaseClient::_readMessage.\n" );
  message_.clear();
  QTextStream stream( &message_, QIODevice::WriteOnly );  
  while( socket().bytesAvailable() > 0 )
  {
    char* data = new char[socket().bytesAvailable()];
    qint64 size = socket().read( data, socket().bytesAvailable() );
    if( size <= 0 ) return;
    stream << QString( data ).left( size );
    delete[] data;
  }

  // store message and emit signal
  has_message_ = true;
  emit messageAvailable();  

}

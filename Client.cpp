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
#include <cassert>

#include "Client.h"
#include "Debug.h"
#include "IOString.h"
#include "ServerXmlDef.h"
#include "XmlError.h"

using namespace std;
using namespace SERVER;


//_______________________________________________________
unsigned int& Client::_counter( void )
{
  static unsigned int counter( 0 );
  return counter;
}

//_______________________________________________________
Client::Client( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "Client" ),
  id_( _counter()++ ),
  socket_( socket )
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
  
  commands_.push_back( command );
  if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendCommands();
  return true;
  
}

//_______________________________________________________
void Client::_sendCommands( void )
{

  if( commands_.empty() ) return;
  
  QDomDocument document;
  QDomElement top = document.appendChild( document.createElement( SERVER_XML::TRANSMISSION ) ).toElement();
  while( commands_.size() && socket().state() == QAbstractSocket::ConnectedState )
  {
    top.appendChild( commands_.front().domElement( document ) );
    commands_.pop_front();
  }
  
  QTextStream os( &socket() );
  os << document.toString() << endl;
  
}

//_______________________________________________________
bool Client::_readMessage( void )
{

  // read everything from socket and store as message
  IOString message( socket() );
  if( message.isEmpty() ) return false;
  
  // add to buffer
  buffer_.append( message );
  
  // parse buffer
  static const QString begin_tag = (QStringList() << "<" << SERVER_XML::TRANSMISSION << ">" ).join("");
  static const QString end_tag = (QStringList() << "</" << SERVER_XML::TRANSMISSION << ">" ).join("");

  while(1)
  {
    
    // get first tag
    int begin_position( _messageBuffer().text().indexOf( begin_tag, _messageBuffer().position() ) );
    if( begin_position < 0 ) break;
    
    // get end tag
    int end_position( _messageBuffer().text().indexOf( end_tag, begin_position+begin_tag.size() ) );
    if( end_position < 0 ) break;
    
    // create QDomDocument
    QString local( _messageBuffer().text().mid( begin_position, end_position+end_tag.size()-begin_position ) );
    
    // create document
    QDomDocument document;
    XmlError error; 
    if ( !document.setContent( local, &error.error(), &error.line(), &error.column() ) ) { Debug::Throw() << error << endl; }
    else {
      
      // parse document 
      QDomElement doc_element = document.documentElement();
      assert( doc_element.tagName() == SERVER_XML::TRANSMISSION );
      
      for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
      {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;
        if( element.tagName() == SERVER_XML::COMMAND ) {

          ServerCommand command( element );
          emit commandAvailable( command.setClientId( id() ) );
          
        } else { Debug::Throw(0) << "ServerCommand::_readMessage - unrecognized tagname: " << element.tagName() << endl; }
        
      }
      
    }
    
    // flush buffer
    _messageBuffer().flush( end_position+end_tag.size() );
      
  }
  
  return true;
  
}

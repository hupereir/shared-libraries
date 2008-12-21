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
   \file    ServerCommand.cpp
   \brief   formated command for interprocess communication
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QStringList>

#include "Debug.h"
#include "ServerCommand.h"
#include "ServerXmlDef.h"
#include "XmlCommandLineArguments.h"

using namespace std;
using namespace SERVER;

//___________________________________________
const QString& ServerCommand::_separator( void )
{
  static const QString sep( "::" );
  return sep;
}

//__________________________________________________________________
ServerCommand::ConversionMap& ServerCommand::_conversions( void )
{
  static ConversionMap conversions;
  return conversions;
}

//__________________________________________________________________
ServerCommand::CommandMap& ServerCommand::_commandNames( void )
{
  static CommandMap names;
  return names;
}

//___________________________________________
void ServerCommand::initializeConversions( void ) const
{
  if( !_conversions().empty() ) return;
  _conversions().insert( std::make_pair( " ", "SERVER_SPACE" ) );
  return;
}

//_________________________________________
void ServerCommand::_initializeCommandNames( void ) const
{
  if( !_commandNames().empty() ) return;
  _commandNames().insert( make_pair( NONE, "" ) );
  _commandNames().insert( make_pair( ACCEPTED, "ACCEPTED" ) );
  _commandNames().insert( make_pair( DENIED, "DENIED" ) );
  _commandNames().insert( make_pair( ABORT, "ABORT" ) );
  _commandNames().insert( make_pair( IDENTIFY, "IDENTIFY" ) );
  _commandNames().insert( make_pair( IDENTIFY_SERVER, "SERVER" ) );
  _commandNames().insert( make_pair( KILLED, "KILLED" ) );
  _commandNames().insert( make_pair( RAISE, "RAISE" ) );
  _commandNames().insert( make_pair( ALIVE, "ALIVE" ) );
  _commandNames().insert( make_pair( REQUEST, "REQUEST" ) );
  _commandNames().insert( make_pair( UNLOCK, "UNLOCK" ) );
}

//___________________________________________
ServerCommand::ServerCommand( const QString& command_line ):
  Counter( "ServerCommand" ),
  timestamp_( TimeStamp::now() ),
  command_( NONE )
{
  
  Debug::Throw( "ServerCommand::ServerCommand.\n" );
  
  // split
  QStringList words( command_line.split( _separator() ) );
  
  // loop over words
  for( int index = 0; index < words.size(); index++ )
  {
    QString word( words[index] );
    switch ( index )
    {
      case 0: id_.setName( word ); break;
      case 1: id_.setUser( word ); break;
      case 2: command_ = (CommandType) word.toUInt(); break;
      default: arguments_ << word; break;
    }
    
  }
  
  Debug::Throw() << "ServerCommand::ServerCommand - id=" << id() << endl;
  Debug::Throw() << "ServerCommand::ServerCommand - command=" << qPrintable( commandName() ) << endl;
    
  return;
  
}

//___________________________________________
ServerCommand::ServerCommand( const QDomElement& element ):
  Counter( "ServerCommand" ),
  timestamp_( TimeStamp::now() ),
  command_( NONE )
{
  
  Debug::Throw( "ServerCommand::ServerCommand (dom).\n" );
  
  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    if( attribute.name() == SERVER_XML::TYPE ) setCommand( (CommandType) attribute.value().toUInt() );
    else Debug::Throw(0) << "ServerCommand::ServerCommand - unrecognized attribute: " << qPrintable( attribute.name() ) << endl;
  }
  
  // loop over children
  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    if( child_element.isNull() ) continue;
    QString tag_name( child_element.tagName() );
    if( tag_name == SERVER_XML::ID ) setId( ApplicationId( child_element ) );
    else if( tag_name == SERVER_XML::ARGUMENTS ) setArguments( XmlCommandLineArguments( child_element ) ); 
  }
  
}

//__________________________________________________
QDomElement ServerCommand::domElement( QDomDocument& document ) const
{

  Debug::Throw( "ServerCommand::domElement.\n" );
  QDomElement out( document.createElement( SERVER_XML::COMMAND ) );
  
  // type
  out.setAttribute( SERVER_XML::TYPE, QString().setNum( command() ) );
  
  // id
  out.appendChild( id().domElement( document ) );
  
  // arguments
  if( !arguments().isEmpty() ) out.appendChild( XmlCommandLineArguments(arguments()).domElement( SERVER_XML::ARGUMENTS, document ) );
  return out;  

}

//__________________________________________________
ServerCommand::operator QString (void) const
{
  Debug::Throw( "ServerCommand::operator (string).\n" );
  if( !id().isValid() ) return "";
  QStringList words;
  
  words << id().name() << id().user() << QString().setNum(command());
  words << arguments();
    
  QString buffer( words.join( _separator() ) );
  
  // apply conversion
  initializeConversions();
  for( ConversionMap::const_iterator iter = ServerCommand::_conversions().begin(); iter != ServerCommand::_conversions().end(); iter++ ) 
  { buffer = buffer.replace( iter->first, iter->second ); }
  return buffer;
  
}

//__________________________________________________
//! create command from stream
namespace SERVER {

  //__________________________________________________
  QTextStream & operator >> ( QTextStream& in, ServerCommand & command )
  {
    
    QString buffer;
    in >> buffer;
    
    // apply conversion
    command.initializeConversions();
    for( 
      ServerCommand::ConversionMap::const_iterator iter = ServerCommand::_conversions().begin();
      iter != ServerCommand::_conversions().end();
      iter++
    ) { buffer = buffer.replace( iter->second, iter->first ); }
    
    command = ServerCommand( buffer );
    return in;
    
  }
    
  //__________________________________________________
  //! dump command to stream
  QTextStream & operator << ( QTextStream& out, const ServerCommand &command )
  {
    
    out << QString( command );
    return out;
    
  }

};

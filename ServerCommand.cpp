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

using namespace std;
using namespace SERVER;

//___________________________________________
const QString ServerCommand::separator_( "::" ); 

//__________________________________________________________________
ServerCommand::ConversionMap ServerCommand::conversions_;

//__________________________________________________________________
ServerCommand::CommandMap ServerCommand::command_names_;

//___________________________________________
void ServerCommand::initializeConversions( void ) const
{
  if( !conversions_.empty() ) return;
  conversions_.insert( std::make_pair( " ", "SERVER_SPACE" ) );
  return;
}

//_________________________________________
void ServerCommand::_initializeCommandNames( void ) const
{
  if( !command_names_.empty() ) return;
  command_names_.insert( make_pair( NONE, "" ) );
  command_names_.insert( make_pair( ACCEPTED, "ACCEPTED" ) );
  command_names_.insert( make_pair( DENIED, "DENIED" ) );
  command_names_.insert( make_pair( ABORT, "ABORT" ) );
  command_names_.insert( make_pair( IDENTIFY, "IDENTIFY" ) );
  command_names_.insert( make_pair( IDENTIFY_SERVER, "SERVER" ) );
  command_names_.insert( make_pair( KILLED, "KILLED" ) );
  command_names_.insert( make_pair( RAISE, "RAISE" ) );
  command_names_.insert( make_pair( ALIVE, "ALIVE" ) );
  command_names_.insert( make_pair( REQUEST, "REQUEST" ) );
  command_names_.insert( make_pair( UNLOCK, "UNLOCK" ) );
}

//___________________________________________
ServerCommand::ServerCommand( const QString& command_line ):
  Counter( "ServerCommand" ),
  timestamp_( TimeStamp::now() ),
  command_( NONE )
{
  
  Debug::Throw( "ServerCommand::ServerCommand.\n" );
  
  // split
  QStringList words( command_line.split( separator_ ) );
  
  // loop over words
  for( int index = 0; index < words.size(); index++ )
  {
    QString word( words[index] );
    switch ( index )
    {
      case 0: id_.setName( word ); break;
      case 1: id_.setUser( word ); break;
      case 2: command_ = (CommandType) word.toUInt(); break;
      default: args_.add( qPrintable( word ) ); break;
    }
    
  }
  
  Debug::Throw() << "ServerCommand::ServerCommand - id=" << id() << endl;
  Debug::Throw() << "ServerCommand::ServerCommand - command=" << qPrintable( commandName() ) << endl;
    
  return;
  
}

//__________________________________________________
ServerCommand::operator QString (void) const
{
  Debug::Throw( "ServerCommand::operator (string).\n" );
  if( !id().isValid() ) return "";
  QStringList words;
  
  words << id().name() << id().user() << QString().setNum(command());
  
  // add arguments  
  for( ArgList::TagList::const_iterator tag_iter = args_.get().begin(); tag_iter != args_.get().end(); tag_iter++ )
  {
    if( tag_iter->tag().size() ) words << tag_iter->tag().c_str();
    for( list<string>::const_iterator opt_iter = tag_iter->options().begin(); opt_iter != tag_iter->options().end(); opt_iter++ )
    { if( opt_iter->size() ) words << opt_iter->c_str(); }
  }
    
  QString buffer( words.join( separator_ ) );
  
  // apply conversion
  initializeConversions();
  for( ConversionMap::const_iterator iter = ServerCommand::conversions_.begin(); iter != ServerCommand::conversions_.end(); iter++ ) 
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
      ServerCommand::ConversionMap::const_iterator iter = ServerCommand::conversions_.begin();
      iter != ServerCommand::conversions_.end();
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

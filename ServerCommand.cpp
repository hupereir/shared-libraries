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

#include "Debug.h"
#include "ServerCommand.h"
#include "Str.h"

using namespace std;
using namespace SERVER;

//___________________________________________
const string ServerCommand::separator_( "::" ); 

//__________________________________________________________________
ServerCommand::ConversionMap ServerCommand::conversions_  = ServerCommand::_initializeConversions();

//__________________________________________________________________
ServerCommand::CommandMap ServerCommand::command_names_;

//___________________________________________
ServerCommand::ConversionMap ServerCommand::_initializeConversions( void )
{
  ConversionMap out;
  out.insert( std::make_pair( " ", "SERVER_SPACE" ) );
  return out;
}

//_________________________________________
void ServerCommand::_initializeCommandNames( void )
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
ServerCommand::ServerCommand( const string& command_line ):
  Counter( "ServerCommand" ),
  timestamp_( TimeStamp::now() ),
  command_( NONE )
{
  
  Debug::Throw( "ServerCommand::ServerCommand.\n" );
  
  int index( 0 );
  size_t pos;
  Str buffer( command_line );
  while( ( pos = buffer.find( separator_ ) ) != string::npos )
  {
    
    Str word( buffer.substr( 0, pos ) );
    switch ( index ){
      case 0: id_.setName( word ); break;
      case 1: id_.setUser( word ); break;
      case 2: command_ = (CommandType) word.get<unsigned int>(); break;
      default: args_.add( word ); break;
    }
    
    index++;
    buffer = buffer.substr( pos+separator_.size(), buffer.size()-pos-separator_.size() );

  }

  // see if there is remaining keyword to find
  if( buffer.size() )
  switch( index ) {
    case 0: id_.setName( buffer ); break;
    case 1: id_.setUser( buffer ); break;
    case 2: command_ = (CommandType) buffer.get<unsigned int>(); break;
    default: args_.add( buffer ); break;
  }
  
  Debug::Throw() << "ServerCommand::ServerCommand - id=" << id() << endl;
  Debug::Throw() << "ServerCommand::ServerCommand - command=" << commandName() << endl;
    
  return;
  
}

//__________________________________________________
ServerCommand::operator std::string (void) const
{
  Debug::Throw( "ServerCommand::operator (string).\n" );
  if( !id().isValid() ) return "";
  Str buffer = id().name() + ServerCommand::separator_ + id().user() + ServerCommand::separator_ + Str().assign<CommandType>(command());
  
  // add arguments  
  for( ArgList::TagList::const_iterator tag_iter = args_.get().begin(); tag_iter != args_.get().end(); tag_iter++ )
  {
    if( tag_iter->tag().size() ) buffer += ServerCommand::separator_ + tag_iter->tag();
    for( list<string>::const_iterator opt_iter = tag_iter->options().begin(); opt_iter != tag_iter->options().end(); opt_iter++ )
    if( opt_iter->size() ) buffer += ServerCommand::separator_ + *opt_iter;
  }
    
  // apply conversion
  for( 
      ServerCommand::ConversionMap::const_iterator iter = ServerCommand::conversions_.begin();
      iter != ServerCommand::conversions_.end();
      iter++
  ) 
  buffer = buffer.replace( iter->first, iter->second );
  return buffer;
}

//__________________________________________________
//! create command from stream
namespace SERVER {

  istream & operator >> ( istream& in, ServerCommand & command )
  {
    
    Str buffer;
    in >> buffer;
    
    // apply conversion
    for( 
      ServerCommand::ConversionMap::const_iterator iter = ServerCommand::conversions_.begin();
      iter != ServerCommand::conversions_.end();
      iter++
        ) 
    { buffer = buffer.replace( iter->second, iter->first ); }
    
    command = ServerCommand( buffer );
    return in;
    
  }
  
  //__________________________________________________
  //! dump command to stream
  ostream & operator << ( ostream& out, const ServerCommand &command )
  {
    
    out << string( command );
    return out;
    
  }
};

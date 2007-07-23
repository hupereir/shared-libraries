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
   \file    ServerCommand.cc
   \brief   formated command for interprocess communication
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Debug.h"
#include "ServerCommand.h"
#include "Str.h"
#include "SERVER.h"

using namespace std;
using namespace SERVER;

//___________________________________________
const string ServerCommand::separator_( "::" ); 

//_________________________________________
const string ServerCommand::ACCEPTED( "ACCEPTED" );
const string ServerCommand::DENIED( "DENIED" );
const string ServerCommand::ABORT( "ABORT" );
const string ServerCommand::IDENTIFY( "IDENTIFY" );
const string ServerCommand::IDENTIFY_SERVER( "SERVER" );
const string ServerCommand::KILLED( "KILLED" );
const string ServerCommand::RAISE( "RAISE" );
const string ServerCommand::ALIVE( "ALIVE" );
const string ServerCommand::REQUEST( "REQUEST" );
const string ServerCommand::UNLOCK( "UNLOCK" );
ServerCommand::ConversionMap ServerCommand::conversions_  = ServerCommand::_initConversions();

//___________________________________________
ServerCommand::ConversionMap ServerCommand::_initConversions( void )
{
  ConversionMap out;
  out.insert( std::make_pair( " ", "SERVER_SPACE" ) );
  return out;
}

//___________________________________________
ServerCommand::ServerCommand( const string& command_line ):
  Counter( "ServerCommand" ),
  command_( "" )
{
  
  Debug::Throw( DEBUG_LEVEL, "ServerCommand::ServerCommand.\n" );
  
  int index( 0 );
  size_t pos;
  string buffer( command_line );
  while( ( pos = buffer.find( separator_ ) ) != string::npos )
  {
    
    string word( buffer.substr( 0, pos ) );
    switch ( index ){
      case 0: id_.setName( word ); break;
      case 1: id_.setUser( word ); break;
      case 2: command_ = word; break;
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
    case 2: command_ = buffer; break;
    default: args_.add( buffer ); break;
  }
  
  Debug::Throw() << "ServerCommand::ServerCommand - id=" << id() << endl;
  Debug::Throw() << "ServerCommand::ServerCommand - command=" << command() << endl;
    
  return;
  
}

//__________________________________________________
ServerCommand::operator std::string (void) const
{
  Debug::Throw( "ServerCommand::operator (string).\n" );
  if( !id().isValid() ) return "";
  Str buffer = id().name() + ServerCommand::separator_ + id().user();
  
  if( command_.size() ) buffer +=  ServerCommand::separator_ + command_;
  
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
istream & SERVER::operator >> ( istream& in, ServerCommand & command )
{

  Str buffer;
  in >> buffer;
  
  // apply conversion
  for( 
      ServerCommand::ConversionMap::const_iterator iter = ServerCommand::conversions_.begin();
      iter != ServerCommand::conversions_.end();
      iter++
  ) buffer = buffer.replace( iter->second, iter->first );
        
  command = ServerCommand( buffer );
  return in;

}

//__________________________________________________
//! dump command to stream
ostream & SERVER::operator << ( ostream& out, const ServerCommand &command )
{

  out << string( command );
  return out;

}

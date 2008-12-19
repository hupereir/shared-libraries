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
  \file CommandLineParser.cpp
  \brief stores command line arguments
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include "CommandLineParser.h"

using namespace std;

//________________________________________________________
CommandLineParser::CommandLineParser( void ):
  Counter( "CommandLineParser" )
{ Debug::Throw( "CommandLineParser::CommandLineParser.\n" ); }

//________________________________________________________
void CommandLineParser::registerFlag( QString tag_name, QString help_text )
{ flags_.insert( make_pair( tag_name, Flag( help_text ) ) ); }

//________________________________________________________
void CommandLineParser::registerOption( QString tag_name, QString type, QString help_text )
{ options_.insert( make_pair( tag_name, Option( type, help_text ) ) ); }

//________________________________________________________
void CommandLineParser::usage( void ) const
{

  Debug::Throw( "CommandLineParser::usage.\n" );
  
  // print flags
  for( FlagMap::const_iterator iter = flags_.begin(); iter != flags_.end(); iter++ )
  { cout << "  " << qPrintable( iter->first ) << "\t\t" << qPrintable( iter->second.help_text_ ) << endl; }
  
  // print options
  for( OptionMap::const_iterator iter = options_.begin(); iter != options_.end(); iter++ )
  { cout << "  " << qPrintable( iter->first ) << " " << qPrintable( iter->second.type_ ) << "\t\t" << qPrintable( iter->second.help_text_ ) << endl; }
  
  return;
}

//________________________________________________________
void CommandLineParser::parse( const CommandLineArguments& arguments )
{
  Debug::Throw( "CommandLineParser::parse.\n" );

  // clear all
  clear();
  
  // check size
  if( arguments.isEmpty() ) return;

  application_name_ = arguments[0];
  for( int index = 1; index < arguments.size(); index++ )
  {
    
    QString tag_name( arguments[index] );
    if( tag_name.isEmpty() ) continue;
    
    // see if tag is in flag list
    {
      FlagMap::iterator iter( flags_.find( tag_name ) );
      if( iter != flags_.end() ) 
      {
        _discardOrphans();
        iter->second.set_ = true;
        continue;
      }
    }
    
    // see if tag is in option list
    {
      OptionMap::iterator iter( options_.find( tag_name ) );
      if( iter != options_.end() )
      {
        if( index+1 < arguments.size() )
        {
          QString value( arguments[index+1] );
          if( !( value.isEmpty() || _isTag(value) ) ) 
          {
            _discardOrphans();
            iter->second.set_ = true;
            iter->second.value_ = value;
            index++;
          } 
        }
        
        Debug::Throw(0) << "CommandLineParser::parse -"
          << " expected argument of type " << qPrintable( iter->second.type_ )
          << " after option " << qPrintable( iter->first ) << endl;
        continue;
      }
    }
    
    // see if tag is an option
    if( _isTag( tag_name ) )
    {
      Debug::Throw(0) << "CommandLineParser::parse - unrecognized option " << qPrintable( tag_name ) << endl;
      continue;
    }
    
    // add to orphans
    orphans_.push_back( tag_name );
  
  }
  
  return;
  
}

//________________________________________________
void CommandLineParser::clear( void )
{
  
  Debug::Throw( "CommandLineParser::clear.\n" );
  
  application_name_.clear();
  orphans_.clear();

  // clear flags
  for( FlagMap::iterator iter = flags_.begin(); iter != flags_.end(); iter++ )
  { iter->second.set_ = false; }
  
  // clear options
  for( OptionMap::iterator iter = options_.begin(); iter != options_.end(); iter++ )
  { 
    iter->second.set_ = false;
    iter->second.value_.clear();
  }

}

//_______________________________________________________
CommandLineArguments CommandLineParser::arguments( void ) const
{ 
  CommandLineArguments out;
  
  // add flags
  for( FlagMap::const_iterator iter = flags_.begin(); iter != flags_.end(); iter++ )
  { if( iter->second.set_ ) out << iter->first; }
  
  // add options
  for( OptionMap::const_iterator iter = options_.begin(); iter != options_.end(); iter++ )
  { if( iter->second.set_ && !iter->second.value_.isEmpty() ) out << iter->first << iter->second.value_; }
    
  // add orphans
  for( QStringList::const_iterator iter = orphans_.begin(); iter != orphans_.end(); iter++ )
  { out << *iter; }
  
  return out;

}
  
//_______________________________________________________
bool CommandLineParser::hasFlag( QString tag ) const
{
  FlagMap::const_iterator iter( flags_.find( tag ) );
  return iter != flags_.end() && iter->second.set_; 
}

//_______________________________________________________
bool CommandLineParser::hasOption( QString tag ) const
{
  OptionMap::const_iterator iter( options_.find( tag ) );
  return iter != options_.end() && iter->second.set_ && !iter->second.value_.isEmpty(); 
}

//_______________________________________________________
QString CommandLineParser::option( QString tag ) const
{
  OptionMap::const_iterator iter( options_.find( tag ) );
  assert( iter != options_.end() && iter->second.set_ && !iter->second.value_.isEmpty() );
  return iter->second.value_;
}

//_______________________________________________________
void CommandLineParser::_discardOrphans( void )
{
  // print discarded orphans
  if( orphans_.isEmpty() ) return;

  Debug::Throw(0) << "CommandLineParser::parse - following orphans are discarded: " << endl;
  for( QStringList::const_iterator iter = orphans_.begin(); iter != orphans_.end(); iter++ )
  { cout << "  " << qPrintable( *iter ) << endl; }

  orphans_.clear();
}

//_______________________________________________________
bool CommandLineParser::_isTag( QString tag ) const
{ return (!tag.isEmpty()) && tag.left(1) == "-"; }

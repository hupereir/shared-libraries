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

#ifndef ArgList_h
#define ArgList_h




/*!
  \file ArgList.h
  \brief parse command line arguments
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "Counter.h"
#include "Debug.h"

/*! \brief 
  parse command line argument.
  first argument is ignored (application name)
  the raw argument list can be accessed as well as
  tagged [using - or --] arguments and associated options
*/
class ArgList: public Counter
{

  public:
  
  //! Tag short cut
  typedef std::string Tag;
  
  //! used to store options which do not come after a tag
  static const Tag NULL_TAG;
 
  //! options associated with a -- argument
  class Arg: public Counter
  {
    
    public:
    
    //! constructor
    Arg( const Tag& tag = NULL_TAG ):
      Counter( "ArgList::Arg" ),
      tag_( tag )
    { addOption(""); }
    
    //! gets tag
    const ArgList::Tag& tag( void ) const
    { return tag_; }
    
    //! string equal_to operator
    bool operator == (const ArgList::Tag& tag ) const
    { return tag_ == tag; }
    
    //! add an option
    void addOption( const std::string& option )
    { 
      Debug::Throw( "ArgList::Arg::addOption.\n" );
      if( std::find( options_.begin(), options_.end(), option ) == options_.end() )
      options_.push_back( option ); 
    }
    
    //! retrieve options
    std::list< std::string >& options( void )
    { return options_; }
     
    //! retrieve options
    const std::list< std::string >& options( void ) const
    { return options_; }
   
    private:
    
    //! tag name
    ArgList::Tag tag_;
    
    //! list of options
    std::list< std::string > options_;
  
    //! dump to stream
    friend std::ostream &operator << (std::ostream &out,const ArgList &arg_list);   
    
  };
  
  //! shortcut to Tag list
  typedef std::list< Arg > TagList;
  
  //! constructor
  ArgList( int argc = 0, char* argv[] = 0 );  

  //! add command line option
  void add( std::string argv );
  
  //! returns true if argument is a tag
  static bool isTag( std::string arg )
  { return arg.substr(0,1) == "-"; }
  
  //! returns true if tag is found in the map
  bool find( const Tag& tag ) const
  { return std::find( tag_list_.begin(), tag_list_.end(), tag ) != tag_list_.end(); }
  
  //! returns list of options associated to a tag
  Arg& get( const Tag& tag )
  {
    Debug::Throw( "ArgList::Get.\n" );
    assert( find( tag ) );
    return *std::find( tag_list_.begin(), tag_list_.end(), tag );
  }
  
  //! returns list of options associated to a tag
  const Arg& get( const Tag& tag ) const
  {
    Debug::Throw( "ArgList::Get [const].\n" );
    assert( find( tag ) );
  }
  
  //! return all arguments
  const TagList& get( void ) const
  { return tag_list_; }
  
  //! return all arguments
  TagList& get( void )
  { return tag_list_; }
  
  //! get last argument
  Tag& last( void )
  { return tag_list_.back().options().back(); }    
  
  //! get last argument
  const Tag& last( void ) const
  { return tag_list_.back().options().back(); }    
  
  private:
  
  //! remove empty strings from arguments
  void _clean( void );
      
  //! tag map
  TagList tag_list_;
  
  //! last entered tag
  Tag last_tag_;
  
  //! dump to stream
  friend std::ostream &operator << (std::ostream &out,const ArgList &arg_list);   
  
};

#endif

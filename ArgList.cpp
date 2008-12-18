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
  \file ArgList.cpp
  \brief parse command line arguments
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ArgList.h"
#include "Debug.h"

using namespace std;

//______________________________________________________
const QString ArgList::NULL_TAG = "";

//______________________________________________________
ArgList::ArgList( int argc, char* argv[] ):
  Counter( "ArgList" ),
  last_tag_( ArgList::NULL_TAG )
{
  Debug::Throw( "ArgList::ArgList.\n" );

  // add NULL_TAG argument
  tag_list_.push_back( Arg( NULL_TAG ) );
  for( int i = 1; i<argc; i++ )
  if( argv[i] ) add( argv[i] );
  
};
  
//______________________________________________________
void ArgList::add( QString arg )
{
  Debug::Throw( "ArgList::add.\n" );
  if( arg.isEmpty() ) return;
      
  // check if argument is a tag name
  if( isTag( arg ) ) 
  {
    
    // check if arg contains "=" sign
    int position( arg.indexOf( "=" ) );
    QString arg_option;
    if( position >= 0 )
    {
      arg_option = arg.mid( position+1 );
      arg = arg.left( position );
    }
    
    last_tag_ = arg;
    if( !find( arg ) ) tag_list_.push_back( Arg( arg ) );
    
    add( arg_option );
    
  } else get( last_tag_ ).addOption( arg );
  
  _clean();

}

//______________________________________________________
//! dymp argument list to stream
std::ostream &operator << (std::ostream &out,const ArgList::Arg &arg)
{

  // dump tagged options
  out << arg.tag() << " ";
  for( list< QString >::const_iterator opt_iter = arg.options().begin(); opt_iter != arg.options().end(); opt_iter++ )
  out << *opt_iter << " ";
  out << endl;
  return out;  

}

//______________________________________________________
//! dymp argument list to stream
std::ostream &operator << (std::ostream &out,const ArgList &arg_list)
{

  // dump tagged options
  for( ArgList::TagList::const_iterator iter = arg_list.tag_list_.begin(); iter != arg_list.tag_list_.end(); iter++ )
  out << *iter << " ";
  out << endl;
  return out;  

}

//______________________________________________________
void ArgList::_clean( void )
{
  
  Debug::Throw( "ArgList::_clean.\n" );
  for( TagList::iterator iter = tag_list_.begin(); iter != tag_list_.end(); iter++ )
  {
    const list<QString> &options( iter->options() );
    list<QString> tmp;
    for( list<QString>::const_iterator opt_iter = options.begin(); opt_iter != options.end(); opt_iter++ )
    if( opt_iter->size() ) tmp.push_back( *opt_iter );
    if( tmp.empty() ) tmp.push_back( "" );    
    iter->options() = tmp;  
  }
  
  return;
  
}

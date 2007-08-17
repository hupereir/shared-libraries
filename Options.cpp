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
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *
 *******************************************************************************/

/*!
  \file Options.cc
  \brief Option file parser based on xml
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <fstream>
#include <list>

#include "Debug.h"
#include "Options.h"

using namespace std;

//________________________________________________
Options::Options( void ):
  Counter( "Options" ),
  options_(),
  special_options_()
{

  /* 
    generic options (default values)
    common to all applications
  */
  add( Option( "DEFAULT_ICON_PATH", "/usr/share/icons" , "default path to look for icons"));  
  add( Option( "DEBUG_LEVEL", "0" , "debug verbosity level"  )); 
  add( Option( "FIXED_FONT_NAME", "Sans" , "fixed font"  )); 
  add( Option( "FONT_NAME", "Sans" , "main font"  )); 
  add( Option( "FONT_SIZE", "10" , "font size"));  
  add( Option( "HIGHLIGHT_PARAGRAPH", "0", "enable paragraph highlighting" ) ); 
  add( Option( "HIGHLIGHT_COLOR", "#fffdd4", "current paragraph highlight color" ) ); 
  add( Option( "ITEM_COLOR", "#f5f5f5", "background color for even items in ListBox and ListView" ) ); 
  add( Option( "USE_BIG_PIXMAP", "1" , "big pixmaps in tool buttons"  )); 
  add( Option( "USE_TEXT_LABEL", "0" , "text label in tool buttons"  )); 
  add( Option( "LOCK_TOOLBARS", "0" , "lock toolbars position" ));
  add( Option( "SORT_FILES_BY_DATE", "0" , "sort files by access date in open previous menu"  )); 
  add( Option( "TAB_EMULATION", "1" , "enable tab emulation" ));
  add( Option( "TAB_SIZE", "2" , "emulated tab size" ));
  add( Option( "WRAP_TEXT", "0" , "text wrapping" ));

}

//____________________________________________________
Option& Options::getOption( const string& name )
{
  OptionMap::iterator iter( options_.find( name ) );
  if( iter == options_.end() ) {
    std::ostringstream what;
    what << "option named " << name << " not found";
    std::cout << DESCRIPTION( what.str() ) << std::endl;
  }

  return iter->second;
}

//________________________________________________
void Options::clearSpecialOptions( const string& name )
{

  if( special_options_.find( name ) == special_options_.end() ) return;
  special_options_[name].clear();
}

//________________________________________________
bool Options::add( const Option& option, bool overwrite )
{

  // store option as special if requested
  SpecialOptionMap::iterator iter( special_options_.find( option.name() ) );
  if( iter != special_options_.end() )
  {
    OptionList::iterator same_option_iter = std::find( iter->second.begin(), iter->second.end(), option );
    if( same_option_iter != iter->second.end() )
    {
      if( option.front() && !same_option_iter->front() )
      {
        iter->second.erase( same_option_iter );
        iter->second.push_front( option );
        return true;
      } else return false;
    }
    if( option.front() ) iter->second.push_front( option );
    else iter->second.push_back( option );
    return true;
  }

  // check if option is already in the map and should not be overwritten
  if( !( options_.find( option.name() ) == options_.end()|| overwrite ) )
  return false;

  // insert option in map, possibly overwriting existing
  options_[option.name()] = option;
  return true;

}

//________________________________________________
void Options::dump( ostream& out ) const
{

  // dump normal options
  for( OptionMap::const_iterator iter = options_.begin(); iter != options_.end(); iter++ )
  out << iter->second << endl;

  // write special options
  for( SpecialOptionMap::const_iterator iter = special_options_.begin(); iter != special_options_.end(); iter++ )
  {
    OptionList option_list( iter->second );
    for( OptionList::iterator list_iter = option_list.begin(); list_iter != option_list.end(); list_iter++ )
    if( list_iter->isRecordable() && list_iter->set() && list_iter->raw().size() )
    out << *list_iter << endl;
  }

}

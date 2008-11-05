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
  \file Options.cpp
  \brief Option file parser based on xml
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <algorithm>
#include <fstream>
#include <list>

#include "Debug.h"
#include "Options.h"

using namespace std;

//________________________________________________
Options::Options( bool install_default_options ):
  Counter( "Options" ),
  options_(),
  special_options_()
{

  if( install_default_options )
  {
    /* 
    generic options (default values)
    common to all applications
    */
    keep( "PIXMAP_PATH" );
    add( "PIXMAP_PATH", Option( ":/pixmaps/oxygen" ) );
    add( "PIXMAP_PATH", Option( ":/pixmaps/crystal" ) );
    add( "PIXMAP_PATH", Option( ":/pixmaps" ) );
    
    add( "DEBUG_LEVEL", Option( "0" , "debug verbosity level" ) ); 
    add( "SORT_FILES_BY_DATE", Option( "0" , "sort files by access date in open previous menu" ) ); 
    add( "USE_FLAT_THEME", Option( "0", "use flat theme in replacement of plastique theme" ) );
    
    // fonts
    add( "FIXED_FONT_NAME", Option( "Sans" , "fixed font" ) ); 
    add( "FONT_NAME", Option( "Sans" , "main font" ) ); 
    
    // toolbars default configuration
    add( "TOOLBUTTON_ICON_SIZE", Option( "32" , "text label in tool buttons" ) ); 
    add( "TOOLBUTTON_TEXT_POSITION", Option( "0" , "text label in tool buttons" ) ); 
    add( "LOCK_TOOLBARS", Option( "0" , "lock toolbars position" ) );
    
    // text editors default configuration
    add( "TAB_EMULATION", Option( "1" , "enable tab emulation" ) );
    add( "TAB_SIZE", Option( "2" , "emulated tab size" ) );
    add( "WRAP_TEXT", Option( "0" , "text wrapping" ) );
    add( "SHOW_LINE_NUMBERS", Option( "0" , "line numbers" ) );
    
    add( "HIGHLIGHT_PARAGRAPH", Option( "1", "enable paragraph highlighting" ) ); 
    add( "HIGHLIGHT_COLOR", Option( "#fffdd4", "current paragraph highlight color" ) ); 
    
    // list configuration
    add( "ALTERNATE_COLOR", Option( "None", "background color for even items in ListBox and ListView" ) ); 
    add( "SELECTED_COLUMN_COLOR", Option( "#fffdd4", "background color for selected column" ) ); 
    add( "LIST_ICON_SIZE", Option( "24", "default icon size in lists" ) );
    
    // textEditor margins
    add( "MARGIN_FOREGROUND", "#136872" );
    add( "MARGIN_BACKGROUND", "#ecffec" );
    add( "MARGIN_VERTICAL_LINE", "1" );
    
    
    // box selection
    add( "BOX_SELECTION_ALPHA", Option( "20", "alpha threshold for box selection - between 0 and 100" ) );
    
  }
  
}

//____________________________________________________
Option& Options::option( const string& name )
{
  Map::iterator iter( options_.find( name ) );
  
  if( iter == options_.end() ) 
  { Debug::Throw(0) << "Options::option - unable to find option named " << name << endl; }
  assert( iter != options_.end() );

  return iter->second;
}

//________________________________________________
void Options::clearSpecialOptions( const string& name )
{

  if( special_options_.find( name ) == special_options_.end() ) return;
  special_options_[name].clear();
}

//________________________________________________
bool Options::add( const std::string& name, const Option& option, bool overwrite )
{
  
  // store option as special if requested
  SpecialMap::iterator iter( special_options_.find( name ) );
  if( iter != special_options_.end() )
  {
    
    List::iterator same_option_iter = std::find( iter->second.begin(), iter->second.end(), option );
    if( same_option_iter != iter->second.end() )
    {
      
      if( option.hasFlag( Option::DEFAULT ) && !same_option_iter->hasFlag( Option::DEFAULT ) )
      {
        
        iter->second.erase( same_option_iter );
        iter->second.insert( iter->second.begin(), option );
        return true;
        
      } else return false;
    
    }
    
    if( option.hasFlag( Option::DEFAULT ) && !iter->second.empty() ) 
    {
      
      iter->second.front().setFlag( Option::DEFAULT, 0 );
      iter->second.insert( iter->second.begin(), option );
      
    } else iter->second.push_back( option );
    
    return true;
    
  }

  // check if option is already in the map and should not be overwritten
  if( !( options_.find( name ) == options_.end() || overwrite ) )
  return false;

  // insert option in map, possibly overwriting existing
  options_[name] = option;
  return true;

}

//________________________________________________
void Options::dump( ostream& out ) const
{
  Debug::Throw( "Options::dump.\n" );
  
  // dump normal options
  for( Map::const_iterator iter = options_.begin(); iter != options_.end(); iter++ )
  out << "  " << iter->first << ":" << iter->second << endl;

  // write special options
  for( SpecialMap::const_iterator iter = special_options_.begin(); iter != special_options_.end(); iter++ )
  {
    List option_list( iter->second );
    for( List::iterator list_iter = option_list.begin(); list_iter != option_list.end(); list_iter++ )
    {
      if( list_iter->hasFlag( Option::RECORDABLE ) && list_iter->set() && list_iter->raw().size() )
      { out << "  " << iter->first << ":" << *list_iter << endl; }
    }
  }
  
  out << endl;
  
}

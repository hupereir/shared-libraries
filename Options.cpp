
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

#include "Debug.h"
#include "Options.h"

#include <algorithm>
#include <list>

//________________________________________________
Options::Options( bool install_default_options ):
    Counter( "Options" ),
    options_(),
    specialOptions_(),
    autoDefault_( false )
{

    if( install_default_options )
    {

        setAutoDefault( true );

        /*
        generic options (default values)
        common to all applications
        */
        keep( "PIXMAP_PATH" );
        add( "PIXMAP_PATH", Option( ":/pixmaps", Option::RECORDABLE|Option::CURRENT ) );

        set( "DEBUG_LEVEL", Option( "0" , "Debug verbosity level" ) );
        set( "SORT_FILES_BY_DATE", Option( "0" , "Sort files by access date in open previous menu" ) );
        set( "USE_FLAT_THEME", Option( "0", "Use flat theme in replacement of plastique theme" ) );

        // fonts
        set( "FIXED_FONT_NAME", Option( "Sans" , "Fixed font (used for editors)" ) );
        set( "FONT_NAME", Option( "Sans" , "Main font (used for all widgets but editors)" ) );

        // toolbars default configuration
        set( "TOOLBUTTON_ICON_SIZE", Option( "32" , "Icon size used for tool buttons" ) );
        set( "TOOLBUTTON_TEXT_POSITION", Option( "0" , "Text label position in tool buttons" ) );

        // text editors default configuration
        set( "TAB_EMULATION", Option( "1" , "Emulate tabs with space characters in text editors" ) );
        set( "TAB_SIZE", Option( "2" , "Emulated tab size" ) );
        set( "WRAP_TEXT", Option( "0" , "Text wrapping" ) );
        set( "SHOW_LINE_NUMBERS", Option( "0" , "Display line numbers on the side of text editors" ) );
        set( "AUTOHIDE_CURSOR", "1" );
        set( "AUTOHIDE_CURSOR_DELAY", "5" );

        set( "HIGHLIGHT_PARAGRAPH", Option( "1", "Enable current paragraph highlighting" ) );
        set( "HIGHLIGHT_COLOR", Option( "#fffdd4", "Current paragraph highlight color" ) );

        // list configuration
        set( "USE_ALTERNATE_COLOR", "0" );
        set( "SELECTED_COLUMN_COLOR", Option( "#fffdd4", "Background color for selected column in list" ) );
        set( "LIST_ICON_SIZE", Option( "24", "Default icon size in lists" ) );

        // textEditor margins
        set( "MARGIN_FOREGROUND", "#136872" );
        set( "MARGIN_BACKGROUND", "#ecffec" );
        set( "MARGIN_VERTICAL_LINE", "1" );

        set( "ANIMATION_FRAMES", "1000" );

        // smooth transitions (disabled under X11 as they usually are handled by the widget style)
        #if defined(Q_WS_X11)
        set( "SMOOTH_TRANSITION_ENABLED", "0" );
        #else
        set( "SMOOTH_TRANSITION_ENABLED", "1" );
        #endif

        set( "SMOOTH_TRANSITION_DURATION", "300" );

        // smooth scrolling
        set( "SMOOTH_SCROLLING_ENABLED", "1" );
        set( "SMOOTH_SCROLLING_DURATION", "100" );

        // box selection
        set( "BOX_SELECTION_ALPHA", Option( "20", "Alpha threshold for box selection - between 0 and 100" ) );

        setAutoDefault( false );

    }

}

//________________________________________________
bool Options::isSpecialOption( const QString& name ) const
{ return specialOptions_.find( name ) != specialOptions_.end(); }

//________________________________________________
void Options::clearSpecialOptions( const QString& name )
{
    assert( isSpecialOption( name ) );
    specialOptions_[name].clear();
}

//________________________________________________
void Options::set( const QString& name, Option option, const bool& is_default )
{
    Debug::Throw() << "Options::set - name: " << name << endl;
    assert( !isSpecialOption( name ) );
    if( is_default || _autoDefault() ) option.setDefault();
    options_[name] = option;
}

//________________________________________________
bool Options::add( const QString& name, Option option, const bool& is_default )
{

    Debug::Throw() << "Options::add - name: " << name << endl;

    // store option as special if requested
    SpecialMap::iterator iter( specialOptions_.find( name ) );

    // check option
    if( iter == specialOptions_.end() ) { QTextStream( stdout ) << "Options::add - invalid option: " << name << endl; }
    assert( iter != specialOptions_.end() );

    // set as default
    if( is_default || _autoDefault() ) option.setDefault();

    // if option is first, set as current
    if( iter->second.empty() ) option.setCurrent( true );
    else if( option.isCurrent() )
    {

        // set all remaining options to non default
        for( List::iterator option_iter = iter->second.begin(); option_iter != iter->second.end(); option_iter ++ )
        { option_iter->setCurrent( false ); }

    }

    // see if option is already in list
    List::iterator same_option_iter = std::find( iter->second.begin(), iter->second.end(), option );
    if( same_option_iter != iter->second.end() )
    {

        // if flags are identical, do nothing and return false
        if( same_option_iter->flags() == option.flags() ) return false;
        else {

            // update flags otherwise and return true
            same_option_iter->setFlags( option.flags() );
            std::sort( iter->second.begin(), iter->second.end(), Option::HasFlagFTor( Option::CURRENT ) );
            return true;

        }

    } else {

        iter->second.push_back( option );
        std::sort( iter->second.begin(), iter->second.end(), Option::HasFlagFTor( Option::CURRENT ) );
        return true;

    }


}

//________________________________________________
void Options::restoreDefaults( void )
{

    // restore standard options
    for( Map::iterator iter = options_.begin(); iter != options_.end(); ++iter )
    {
        if( iter->second.defaultValue().isEmpty() ) continue;
        iter->second.restoreDefault();
    }

    // restore standard options
    for( SpecialMap::iterator iter = specialOptions_.begin(); iter != specialOptions_.end(); ++iter )
    {
        Options::List option_list( iter->second );
        iter->second.clear();
        for( Options::List::iterator listIter = option_list.begin(); listIter != option_list.end(); ++listIter )
        {
            if( listIter->defaultValue().isEmpty() ) continue;
            add( iter->first, listIter->restoreDefault() );
        }
    }

}

//________________________________________________
QTextStream &operator << ( QTextStream &out,const Options &options)
{

    // print normal options
    for( Options::Map::const_iterator iter = options.options().begin(); iter != options.options().end(); ++iter )
        out << "  " << iter->first << ":" << iter->second << endl;

    // write special options
    for( Options::SpecialMap::const_iterator iter = options.specialOptions().begin(); iter != options.specialOptions().end(); ++iter )
    {
        const Options::List& option_list( iter->second );
        for( Options::List::const_iterator listIter = option_list.begin(); listIter != option_list.end(); ++listIter )
        {
            if( listIter->hasFlag( Option::RECORDABLE ) && listIter->set() && listIter->raw().size() )
            { out << "  " << iter->first << ":" << *listIter << endl; }
        }
    }

    out << endl;
    return out;
}

//________________________________________________
Options::Map::const_iterator Options::_find( const QString& name ) const
{
    Map::const_iterator out( options_.find( name ) );
    if( out == options_.end() ) { QTextStream( stdout ) << "Options::_find - invalid option: " << name << endl; }
    assert( out != options_.end() );
    return out;
}


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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Options.h"

#include "Color.h"
#include "Debug.h"

#include <algorithm>

//________________________________________________
Options::Options( void ):
    Counter( "Options" ),
    autoDefault_( false )
{}

//________________________________________________
void Options::installDefaultOptions( void )
{

    setAutoDefault( true );

    /*
    generic options (default values)
    common to all applications
    */
    keep( "PIXMAP_PATH" );
    add( "PIXMAP_PATH", Option( ":/icons/32x32", Option::Recordable|Option::Current ) );
    add( "PIXMAP_PATH", Option( ":/icons/48x48", Option::Recordable ) );
    add( "PIXMAP_PATH", Option( ":/icons/64x64", Option::Recordable ) );
    add( "PIXMAP_PATH", Option( ":/icons/128x128", Option::Recordable ) );

    set<int>( "DEBUG_LEVEL", 0 );
    set<bool>( "SORT_FILES_BY_DATE", false );

    // style

    #if defined(Q_OS_WIN)
    set<bool>( "FORCE_PLASTIQUE_STYLE", true );
    #else
    set<bool>( "FORCE_PLASTIQUE_STYLE", false );
    #endif

    // fonts
    set<bool>( "USE_SYSTEM_FONT", true );
    set( "FIXED_FONT_NAME", "Sans" );
    set( "FONT_NAME", "Sans" );

    // toolbars default configuration
    set<int>( "TOOLBUTTON_ICON_SIZE", 0 );
    set<int>( "TOOLBUTTON_TEXT_POSITION", -1 );

    // text editors default configuration
    set<bool>( "TAB_EMULATION", true );
    set<int>( "TAB_SIZE", 2 );
    set<bool>( "WRAP_TEXT", false );
    set<bool>( "SHOW_LINE_NUMBERS", false );
    set<bool>( "AUTOHIDE_CURSOR", true );
    set<int>( "AUTOHIDE_CURSOR_DELAY", 5 );

    set<bool>( "HIGHLIGHT_PARAGRAPH", true );
    set<Base::Color>( "HIGHLIGHT_COLOR", QColor( "#fffdd4" ) );

    // help dialog
    set<int>( "HELP_DIALOG_WIDTH", 750 );
    set<int>( "HELP_DIALOG_HEIGHT", 500 );

    // list configuration
    set<bool>( "USE_ALTERNATE_COLOR", false );
    set<bool>( "USE_SELECTED_COLUMN_COLOR", true );

    set<int>( "LIST_ICON_SIZE", 24 );
    set<int>( "LIST_ITEM_MARGIN", 0 );

    // textEditor
    set<Base::Color>( "MARGIN_FOREGROUND", QColor("#136872") );
    set<Base::Color>( "MARGIN_BACKGROUND", QColor("#ecffec") );

    set( "ANIMATION_FRAMES", "1000" );

    // dock widgets
    set<bool>( "FLOATABLE_DOCK_WIDGETS_ENABLED", false );

    // smooth transitions (disabled under X11 as they usually are handled by the widget style)
    #if defined(Q_OS_UNIX)
    set<bool>( "SMOOTH_TRANSITION_ENABLED", false );
    #else
    set<bool>( "SMOOTH_TRANSITION_ENABLED", true );
    #endif

    set<int>( "SMOOTH_TRANSITION_DURATION", 300 );

    // smooth scrolling
    #if defined( Q_OS_MAC )
    set<bool>( "SMOOTH_SCROLLING_ENABLED", false );
    #else
    set<bool>( "SMOOTH_SCROLLING_ENABLED", true );
    #endif

    set<int>( "SMOOTH_SCROLLING_DURATION", 100 );

    // box selection
    set<int>( "BOX_SELECTION_ALPHA", 20 );

    // dictionaries and filter
    set( "SPELLCHECK_DISABLED_DICTIONARIES", " " );
    set( "SPELLCHECK_DISABLED_FILTERS", " " );

    // icon theme
    set<bool>( "USE_ICON_THEME", false );
    set( "ICON_THEME", "oxygen" );
    set( "ICON_THEME_PATH", "/usr/share/icons" );

    // default preferences dialog size
    set<int>( "PREFERENCE_DIALOG_WIDTH", 600 );
    set<int>( "PREFERENCE_DIALOG_HEIGHT", 500 );

    setAutoDefault( false );

}

//________________________________________________
bool Options::isSpecialOption( const QString& name ) const
{ return specialOptions_.find( name ) != specialOptions_.end(); }

//________________________________________________
void Options::clearSpecialOptions( const QString& name )
{
    Q_ASSERT( isSpecialOption( name ) );
    specialOptions_[name].clear();
}

//________________________________________________
void Options::set( const QString& name, const Option& constOption, bool isDefault )
{
    Debug::Throw() << "Options::set - name: " << name << endl;
    Q_ASSERT( !isSpecialOption( name ) );
    Option option( constOption );
    if( isDefault || _autoDefault() ) option.setDefault();
    options_[name] = option;
}

//________________________________________________
bool Options::add( const QString& name, const Option& constOption, bool isDefault )
{

    Debug::Throw() << "Options::add - name: " << name << endl;

    // store option as special if requested
    SpecialMap::iterator iter( specialOptions_.find( name ) );

    // check option
    if( iter == specialOptions_.end() )
    {
        QTextStream( stdout ) << "Options::add - invalid option: " << name << endl;
        return false;
    }

    // set as default
    Option option( constOption );
    if( isDefault || _autoDefault() ) option.setDefault();

    // if option is first, set as current
    if( iter.value().empty() ) option.setCurrent( true );
    else if( option.isCurrent() )
    {

        // set all remaining options to non default
        for( List::iterator option_iter = iter.value().begin(); option_iter != iter.value().end(); option_iter ++ )
        { option_iter->setCurrent( false ); }

    }

    // see if option is already in list
    List::iterator sameOptionIter = std::find_if( iter.value().begin(), iter.value().end(), Option::SameValueFTor( option ) );
    if( sameOptionIter != iter.value().end() )
    {

        // if options are identical, do nothing and return false
        if( *sameOptionIter == option ) return false;
        else {

            // update flags otherwise and return true
            sameOptionIter->setFlags( option.flags() );
            std::sort( iter.value().begin(), iter.value().end(), Option::HasFlagFTor( Option::Current ) );
            return true;

        }

    } else {

        iter.value() << option;
        std::sort( iter.value().begin(), iter.value().end(), Option::HasFlagFTor( Option::Current ) );
        return true;

    }
}

//________________________________________________
void Options::restoreDefaults( void )
{

    // restore standard options
    for( Map::iterator iter = options_.begin(); iter != options_.end(); ++iter )
    {
        if( iter.value().defaultValue().isEmpty() ) continue;
        iter.value().restoreDefault();
    }

    // restore standard options
    for( SpecialMap::iterator iter = specialOptions_.begin(); iter != specialOptions_.end(); ++iter )
    {
        Options::List optionList( iter.value() );
        iter.value().clear();
        for( Options::List::iterator listIter = optionList.begin(); listIter != optionList.end(); ++listIter )
        {
            if( listIter->defaultValue().isEmpty() ) continue;
            add( iter.key(), listIter->restoreDefault() );
        }
    }

}

//________________________________________________
QTextStream &operator << ( QTextStream &out,const Options &options)
{

    // print normal options
    for( Options::Map::const_iterator iter = options.options().begin(); iter != options.options().end(); ++iter )
    { out << "  " << iter.key() << ":" << iter.value() << endl; }

    // write special options
    for( Options::SpecialMap::const_iterator iter = options.specialOptions().begin(); iter != options.specialOptions().end(); ++iter )
    {
        const Options::List& optionList( iter.value() );
        for( Options::List::const_iterator listIter = optionList.begin(); listIter != optionList.end(); ++listIter )
        {
            if( listIter->hasFlag( Option::Recordable ) && listIter->isSet() )
            { out << "  " << iter.key() << ":" << *listIter << endl; }
        }
    }

    out << endl;
    return out;
}

//________________________________________________
Options::Map::const_iterator Options::_find( const QString& name ) const
{
    Map::const_iterator out( options_.find( name ) );
    if( out == options_.end() ) { Debug::Throw(0) << "Options::_find - invalid option: " << name << endl; }
    Q_ASSERT( out != options_.end() );
    return out;
}

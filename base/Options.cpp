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
#include "CppUtil.h"
#include "Debug.h"

#include <algorithm>

//________________________________________________
Options::Options():
    Counter( QStringLiteral("Options") )
{}

//________________________________________________
void Options::installDefaultOptions()
{

    setAutoDefault( true );

    /*
    generic options (default values)
    common to all applications
    */
    keep( QStringLiteral("PIXMAP_PATH") );
    add( QStringLiteral("PIXMAP_PATH"), Option( ":/icons/32x32", Option::Flag::Recordable|Option::Flag::Current ) );
    add( QStringLiteral("PIXMAP_PATH"), Option( ":/icons/48x48", Option::Flag::Recordable ) );
    add( QStringLiteral("PIXMAP_PATH"), Option( ":/icons/64x64", Option::Flag::Recordable ) );
    add( QStringLiteral("PIXMAP_PATH"), Option( ":/icons/128x128", Option::Flag::Recordable ) );

    set<int>( QStringLiteral("DEBUG_LEVEL"), 0 );
    set<bool>( QStringLiteral("SORT_FILES_BY_DATE"), true );

    // style

    #if defined(Q_OS_WIN)
    set<bool>( QStringLiteral("FORCE_PLASTIQUE_STYLE"), true );
    #else
    set<bool>( QStringLiteral("FORCE_PLASTIQUE_STYLE"), false );
    #endif

    // fonts
    set<bool>( QStringLiteral("USE_SYSTEM_FONT"), true );
    set( QStringLiteral("FIXED_FONT_NAME"), "Sans Serif,10" );
    set( QStringLiteral("FONT_NAME"), "Monospace,10" );

    // toolbars default configuration
    set<int>( QStringLiteral("TOOLBUTTON_ICON_SIZE"), 0 );
    set<int>( QStringLiteral("TOOLBUTTON_TEXT_POSITION"), -1 );

    // text editors default configuration
    set<bool>( QStringLiteral("TAB_EMULATION"), true );
    set<int>( QStringLiteral("TAB_SIZE"), 2 );
    set<bool>( QStringLiteral("WRAP_TEXT"), false );
    set<bool>( QStringLiteral("SHOW_LINE_NUMBERS"), false );
    set<int>( QStringLiteral("AUTOHIDE_CURSOR_DELAY"), 5 );

    set<bool>( QStringLiteral("HIGHLIGHT_PARAGRAPH"), true );
    set( QStringLiteral("HIGHLIGHT_COLOR"), Base::Color( "#fffdd4" ) );

    // help dialog
    set<int>( QStringLiteral("HELP_DIALOG_WIDTH"), 750 );
    set<int>( QStringLiteral("HELP_DIALOG_HEIGHT"), 500 );

    // list configuration
    set<bool>( QStringLiteral("USE_ALTERNATE_COLOR"), false );
    set<bool>( QStringLiteral("USE_SELECTED_COLUMN_COLOR"), true );

    /* warning: this must match one of the default icon size */
    set<int>( QStringLiteral("LIST_ICON_SIZE"), 22 );
    set<int>( QStringLiteral("LIST_ITEM_MARGIN"), 0 );

    // box selection
    set<int>( QStringLiteral("BOX_SELECTION_ALPHA"), 20 );

    // dictionaries and filter
    set( QStringLiteral("SPELLCHECK_DISABLED_DICTIONARIES"), " " );
    set( QStringLiteral("SPELLCHECK_DISABLED_FILTERS"), " " );

    // icon theme
    set<bool>( QStringLiteral("USE_ICON_THEME"), false );
    set( QStringLiteral("ICON_THEME"), "oxygen" );
    set( QStringLiteral("ICON_THEME_PATH"), "/usr/share/icons" );

    // default preferences dialog size
    set<int>( QStringLiteral("PREFERENCE_DIALOG_WIDTH"), 600 );
    set<int>( QStringLiteral("PREFERENCE_DIALOG_HEIGHT"), 500 );

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
    Debug::Throw() << "Options::set - name: " << name << Qt::endl;
    auto option( constOption );
    if( isDefault || _autoDefault() ) option.setDefault();

    // options_[name] = option;
    Base::insert( options_, name, option );
}

//________________________________________________
bool Options::add( const QString& name, const Option& constOption, bool isDefault )
{

    Debug::Throw() << "Options::add - name: " << name << " value: \"" << constOption.raw() << "\"" << Qt::endl;

    // store option as special if requested
    auto iter( specialOptions_.find( name ) );

    // check option
    if( iter == specialOptions_.end() )
    {
        QTextStream( stdout ) << "Options::add - invalid option: " << name << Qt::endl;
        return false;
    }

    // set as default
    auto option( constOption );
    if( isDefault || _autoDefault() ) option.setDefault();

    // if option is first, set as current
    if( iter.value().empty() ) option.setCurrent( true );
    else if( option.isCurrent() )
    { for( auto& option:iter.value() ) option.setCurrent( false ); }

    // see if option is already in list
    auto sameOptionIter = std::find_if( iter.value().begin(), iter.value().end(), Option::SameValueFTor( option ) );
    if( sameOptionIter != iter.value().end() )
    {

        // if options are identical, do nothing and return false
        if( *sameOptionIter == option ) return false;
        else {

            // update flags otherwise and return true
            sameOptionIter->setFlags( option.flags() );
            std::partition( iter.value().begin(), iter.value().end(), Option::HasFlagFTor( Option::Flag::Current ) );
            return true;

        }

    } else {

        iter.value().append( option );
        std::partition( iter.value().begin(), iter.value().end(), Option::HasFlagFTor( Option::Flag::Current ) );
        return true;

    }
}

//________________________________________________
void Options::restoreDefaults()
{

    // restore standard options
    for( auto&& iter = options_.begin(); iter != options_.end(); ++iter )
    {
        if( iter.value().defaultValue().isEmpty() ) continue;
        iter.value().restoreDefault();
    }

    // restore standard options
    for( auto&& iter = specialOptions_.begin(); iter != specialOptions_.end(); ++iter )
    {
        auto optionList( iter.value() );
        iter.value().clear();
        for( auto& option:optionList )
        {
            if( option.defaultValue().isEmpty() ) continue;
            add( iter.key(), option.restoreDefault() );
        }
    }

}

//________________________________________________
QTextStream &operator << ( QTextStream &out,const Options &options)
{

    // print normal options
    for( auto&& iter = options.options().begin(); iter != options.options().end(); ++iter )
    { out << "  " << iter.key() << ":" << iter.value() << Qt::endl; }

    // write special options
    for( auto&& iter = options.specialOptions().begin(); iter != options.specialOptions().end(); ++iter )
    {
        const Options::List& optionList( iter.value() );
        for( const auto& option:optionList )
        {
            if( option.hasFlag( Option::Flag::Recordable ) && option.isSet() )
            { out << "  " << iter.key() << ":" << option << Qt::endl; }
        }
    }

    out << Qt::endl;
    return out;
}

//________________________________________________
Options::Map::const_iterator Options::_find( const QString& name ) const
{
    Map::const_iterator out( options_.find( name ) );
    if( out == options_.end() ) { Debug::Throw(0) << "Options::_find - invalid option: " << name << Qt::endl; }
    return out;
}

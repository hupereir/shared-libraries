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

#include "CommandLineParser.h"

#include <QtCore/QTextStream>
#include <algorithm>

//________________________________________________________
CommandLineParser::CommandLineParser( void ):
Counter( "CommandLineParser" )
{
    Debug::Throw( "CommandLineParser::CommandLineParser.\n" );
    registerFlag( "--help", "print this help and exit" );
}

//________________________________________________________
void CommandLineParser::registerFlag( QString tagName, QString helpText )
{ flags_.insert( tagName, Flag( helpText ) ); }

//________________________________________________________
void CommandLineParser::registerOption( QString tagName, QString type, QString helpText )
{ options_.insert( tagName, Option( type, helpText ) ); }

//________________________________________________________
void CommandLineParser::usage( void ) const
{

    Debug::Throw( "CommandLineParser::usage.\n" );

    const QList<QString> flagKeys( flags_.keys() );
    const QList<QString> optionKeys( options_.keys() );

    int max_length = 1+ qMax(
        std::max_element( flagKeys.begin(), flagKeys.end(), MinLengthFTor() )->size(),
        std::max_element( optionKeys.begin(), optionKeys.end(), MinLengthFTor() )->size() );

    const QList<Option> optionValues( options_.values() );
    int max_type_length = 1+ std::max_element( optionValues.begin(), optionValues.end(), MinTypeLengthFTor() )->type_.size();

    QTextStream stream( stdout );

    // print flags
    for( FlagMap::const_iterator iter = flags_.begin(); iter != flags_.end(); ++iter )
    {
        stream << "  ";
        stream << iter.key().leftJustified( max_length + max_type_length + 1 );
        stream << iter.value().helpText_ << endl;
    }

    // print options
    for( OptionMap::const_iterator iter = options_.begin(); iter != options_.end(); ++iter )
    {
        stream << "  ";
        stream << iter.key().leftJustified( max_length );
        stream << iter.value().type_.leftJustified( max_type_length + 1 );
        stream << iter.value().helpText_ << endl;
    }

    stream
        << "Note: special tag \"-\" can be added to separate options from the last list of arguments "
        << "(e.g. list of files to be opened)"
        << endl;

    return;
}

//________________________________________________________
CommandLineParser& CommandLineParser::parse( const CommandLineArguments& arguments, bool ignore_warnings )
{
    Debug::Throw( "CommandLineParser::parse.\n" );

    // clear all
    clear();

    // check size
    if( arguments.isEmpty() ) return *this;

    // first argument is application name
    applicationName_ = arguments[0];

    // check if all options should be orphans
    // this should be triggered by adding "-" in the command line
    bool auto_orphan( false );

    for( int index = 1; index < arguments.size(); index++ )
    {

        QString tagName( arguments[index] );
        if( tagName.isEmpty() ) continue;

        if( tagName == "-" )
        {
            if( auto_orphan && !ignore_warnings )
            {
                QTextStream(stdout) << "CommandLineParser::parse -"
                    << " option delimiter \"-\" appears multiple times "
                    << endl;
            }

            auto_orphan = true;
            continue;
        }

        // see if tag is in flag list
        {
            FlagMap::iterator iter( flags_.find( tagName ) );
            if( iter != flags_.end() )
            {
                if( auto_orphan )
                {
                    if( !ignore_warnings )
                    {
                        QTextStream(stdout) << "CommandLineParser::parse -"
                            << " tag " << tagName << " appears after option delimiter \"-\". It is ignored."
                            << endl;
                    }

                    continue;

                }

                _discardOrphans( ignore_warnings );
                iter.value().set_ = true;
                continue;
            }
        }

        // see if tag is in option list
        {
            OptionMap::iterator iter( options_.find( tagName ) );
            if( iter != options_.end() )
            {

                if( auto_orphan )
                {

                    if( !ignore_warnings )
                    {
                        QTextStream(stdout) << "CommandLineParser::parse -"
                            << " tag " << tagName << " appears after option delimiter \"-\". It is ignored."
                            << endl;
                    }

                    // also skip next entry
                    if( index+1 < arguments.size() ) index++;

                    continue;

                }

                if( index+1 < arguments.size() )
                {

                    QString value( arguments[index+1] );
                    if( !( value.isEmpty() || _isTag(value) ) )
                    {
                        _discardOrphans( ignore_warnings );
                        iter.value().set_ = true;
                        iter.value().value_ = value;
                        index++;
                        continue;
                    }

                } else if( !ignore_warnings ) {

                    Debug::Throw(0) << "CommandLineParser::parse -"
                        << " expected argument of type " << iter.value().type_
                        << " after option " << iter.key() << endl;
                }

                continue;
            }
        }

        // see if tag is an option
        if( _isTag( tagName ) )
        {

            if( !ignore_warnings )
            { Debug::Throw(0) << "CommandLineParser::parse - unrecognized option " << tagName << endl; }

            continue;

        }

        // add to orphans
        orphans_ << tagName;

    }

    return *this;

}

//________________________________________________
void CommandLineParser::clear( void )
{

    Debug::Throw( "CommandLineParser::clear.\n" );

    applicationName_.clear();
    orphans_.clear();

    // clear flags
    for( FlagMap::iterator iter = flags_.begin(); iter != flags_.end(); ++iter )
    { iter.value().set_ = false; }

    // clear options
    for( OptionMap::iterator iter = options_.begin(); iter != options_.end(); ++iter )
    {
        iter.value().set_ = false;
        iter.value().value_.clear();
    }

}

//_______________________________________________________
CommandLineArguments CommandLineParser::arguments( void ) const
{
    Debug::Throw( "CommandLineParser::arguments.\n" );

    CommandLineArguments out;
    out << applicationName_;

    // add flags
    for( FlagMap::const_iterator iter = flags_.begin(); iter != flags_.end(); ++iter )
    { if( iter.value().set_ ) out << iter.key(); }

    // add options
    for( OptionMap::const_iterator iter = options_.begin(); iter != options_.end(); ++iter )
    { if( iter.value().set_ && !iter.value().value_.isEmpty() ) out << iter.key() << iter.value().value_; }

    // add orphans
    for( QStringList::const_iterator iter = orphans_.begin(); iter != orphans_.end(); ++iter )
    { out << *iter; }

    return out;

}

//_______________________________________________________
bool CommandLineParser::hasFlag( QString tag ) const
{
    FlagMap::const_iterator iter( flags_.find( tag ) );
    return iter != flags_.end() && iter.value().set_;
}

//_______________________________________________________
bool CommandLineParser::hasOption( QString tag ) const
{
    OptionMap::const_iterator iter( options_.find( tag ) );
    return iter != options_.end() && iter.value().set_ && !iter.value().value_.isEmpty();
}

//_______________________________________________________
QString CommandLineParser::option( QString tag ) const
{
    OptionMap::const_iterator iter( options_.find( tag ) );
    Q_ASSERT( iter != options_.end() && iter.value().set_ && !iter.value().value_.isEmpty() );
    return iter.value().value_;
}

//_______________________________________________________
void CommandLineParser::_discardOrphans( bool ignore_warnings )
{
    // print discarded orphans
    if( orphans_.isEmpty() ) return;

    if( !ignore_warnings )
    {
        Debug::Throw(0) << "CommandLineParser::parse - following orphans are discarded: " << endl;
        for( QStringList::const_iterator iter = orphans_.begin(); iter != orphans_.end(); ++iter )
        { QTextStream( stdout ) << "  " << *iter << endl; }
    }

    orphans_.clear();
}

//_______________________________________________________
bool CommandLineParser::_isTag( QString tag ) const
{ return (!tag.isEmpty()) && tag.left(1) == "-"; }

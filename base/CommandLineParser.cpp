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

#include "CommandLineParser.h"

#include <QTextStream>
#include <QObject>
#include <algorithm>

//________________________________________________________
const QString CommandLineParser::applicationGroupName( QObject::tr( "Application Options:" ) );
const QString CommandLineParser::serverGroupName( QObject::tr( "Server Options:" ) );
const QString CommandLineParser::qtGroupName( QObject::tr( "Qt Options:" ) );

//________________________________________________________
CommandLineParser::CommandLineParser():
    Counter( QStringLiteral("CommandLineParser") ),
    groupNames_( {applicationGroupName, serverGroupName, qtGroupName} )
{}

//________________________________________________________
void CommandLineParser::usage() const
{

    Debug::Throw( QStringLiteral("CommandLineParser::usage.\n") );

    QTextStream stream( stdout );
    for( const auto& groupName:groupNames_ )
    {
        if( !groups_.contains( groupName ) ) continue;

        stream << Qt::endl;

        // header
        if( groups_.size() == 1 || groupName.isEmpty() ) stream << QObject::tr( "Options:" ) << Qt::endl;
        else stream << groupName << Qt::endl;

        // retrieve group
        const auto& group( groups_[groupName] );

        int maxFlagLength(0);
        if( !group.flags_.isEmpty() )
        {
            const auto flagKeys( group.flags_.keys() );
            maxFlagLength = std::max_element( flagKeys.cbegin(), flagKeys.cend(), MinLengthFTor() )->size();
        }

        int maxOptionLength(0);
        int maxTypeLength(0);
        if( !group.options_.isEmpty() )
        {
            const auto optionKeys( group.options_.keys() );
            const auto optionValues( group.options_.values() );
            maxOptionLength = std::max_element( optionKeys.cbegin(), optionKeys.cend(), MinLengthFTor() )->size();
            maxTypeLength = 1 + std::max_element( optionValues.cbegin(), optionValues.cend(), MinTypeLengthFTor() )->type_.size();
        }

        int maxLength = 1+ qMax( maxFlagLength, maxOptionLength );

        // print flags
        for( auto&& iter = group.flags_.cbegin(); iter != group.flags_.cend(); ++iter )
        {
            stream << "  ";
            stream << iter.key().toString().leftJustified( maxLength + maxTypeLength + 1 );
            stream << iter.value().helpText_ << Qt::endl;
        }

        // print options
        for( auto&& iter = group.options_.cbegin(); iter != group.options_.cend(); ++iter )
        {
            stream << "  ";
            stream << iter.key().toString().leftJustified( maxLength );
            stream << iter.value().type_.leftJustified( maxTypeLength + 1 );
            stream << iter.value().helpText_ << Qt::endl;
        }

    }

    stream
        << Qt::endl
        << QObject::tr( "Special tag \"-\" can be added to separate options from the last list of arguments\n (e.g. list of files to be opened)" )
        << Qt::endl;

    return;
}

//_______________________________________________________
CommandLineArguments CommandLineParser::arguments() const
{
    Debug::Throw( QStringLiteral("CommandLineParser::arguments.\n") );

    CommandLineArguments out( { applicationName_ } );

    // add flags
    for( auto&& iter = groups_.cbegin(); iter != groups_.cend(); ++iter )
    {
        const Group& group = iter.value();

        for( auto&& iter = group.flags_.cbegin(); iter != group.flags_.cend(); ++iter )
        { if( iter.value().set_ ) out.append( iter.key().longName() ); }

        // add options
        for( auto&& iter = group.options_.cbegin(); iter != group.options_.cend(); ++iter )
        { if( iter.value().set_ && !iter.value().value_.isEmpty() ) out.append<QStringList>( { iter.key().longName(), iter.value().value_ } ); }

    }

    // add orphans
    out.append( orphans_ );

    return out;

}

//_______________________________________________________
bool CommandLineParser::hasFlag( const QString& tag ) const
{
    const auto flags( _allFlags() );
    auto&& iter( _findTag( flags, tag ) );
    return iter != flags.end() && iter.value().set_;
}

//_______________________________________________________
bool CommandLineParser::hasOption( const QString& tag ) const
{
    const auto options( _allOptions() );
    auto&& iter( _findTag( options, tag ) );
    return iter != options.end() && iter.value().set_ && !iter.value().value_.isEmpty();
}

//_______________________________________________________
QString CommandLineParser::option( const QString& tag ) const
{
    const auto options( _allOptions() );
    auto&& iter( _findTag( options, tag ) );
    return iter.value().value_;
}

//________________________________________________________
void CommandLineParser::registerFlag( const Tag& tag, const QString& helpText )
{
    if( !_allFlags().contains( tag ) )
    { groups_[currentGroup_].flags_.insert( tag, Flag( helpText ) ); }
}

//________________________________________________________
void CommandLineParser::registerOption( const Tag& tag, const QString& type, const QString& helpText )
{
    if( !_allOptions().contains( tag ) )
    { groups_[currentGroup_].options_.insert( tag, Option( type, helpText ) ); }
}

//________________________________________________________
CommandLineParser& CommandLineParser::parse( const CommandLineArguments& arguments, bool ignoreWarnings )
{
    Debug::Throw( QStringLiteral("CommandLineParser::parse.\n") );

    // clear all
    clear();

    // check size
    if( arguments.isEmpty() ) return *this;

    // first argument is application name
    applicationName_ = arguments[0];

    // check if all options should be orphans
    // this should be triggered by adding "-" in the command line
    bool autoOrphan( false );

    // get all flags and all options
    auto flags( _allFlags() );
    auto options( _allOptions() );

    for( int index = 1; index < arguments.size(); index++ )
    {

        QString tagName( arguments[index] );
        if( tagName.isEmpty() ) continue;

        if( tagName == QLatin1String("-") )
        {
            if( autoOrphan && !ignoreWarnings )
            {
                QTextStream(stdout) << "CommandLineParser::parse -"
                    << " option delimiter \"-\" appears multiple times "
                    << Qt::endl;
            }

            autoOrphan = true;
            continue;
        }

        // see if tag contains "=" string
        QString value;
        auto values( tagName.split( QLatin1Char( '=' ) ) );
        if( values.size() == 2 )
        {

            tagName = values[0];
            value = values[1];

        } else if( values.size() > 2 ) {

            if( !ignoreWarnings )
            {
                QTextStream(stdout)
                    << "CommandLineParser::parse -"
                    << " tag " << tagName
                    << " is ill-formed. It is ignored."
                    << Qt::endl;
            }
            continue;

        }

        // see if tag is in flag list. Only check if no value is found.
        if( value.isEmpty() )
        {
            auto&& iter = _findTag( flags, tagName );
            if( iter != flags.end() )
            {
                if( autoOrphan )
                {
                    if( !ignoreWarnings )
                    {
                        QTextStream(stdout)
                            << "CommandLineParser::parse -"
                            << " tag " << tagName
                            << " appears after option delimiter \"-\". It is ignored."
                            << Qt::endl;
                    }

                    continue;

                }

                _discardOrphans( ignoreWarnings );
                iter.value().set_ = true;

                // also find true option in group lists and set flag
                for( auto&& groupIter = groups_.begin(); groupIter != groups_.end(); ++groupIter )
                {
                    auto&& iter( _findTag( groupIter.value().flags_, tagName ) );
                    if( iter != groupIter.value().flags_.end() )
                    { iter.value().set_ = true; }
                }

                continue;
            }
        }

        // see if tag is in option list
        {
            Group::OptionMap::iterator iter = _findTag( options, tagName );
            if( iter != options.end() )
            {

                if( autoOrphan )
                {

                    if( !ignoreWarnings )
                    {
                        QTextStream(stdout)
                            << "CommandLineParser::parse -"
                            << " tag " << tagName
                            << " appears after option delimiter \"-\". It is ignored."
                            << Qt::endl;
                    }

                    // also skip next entry
                    if( value.isEmpty() && index+1 < arguments.size() ) index++;

                    continue;

                }

                if( value.isEmpty() && index+1 < arguments.size() )
                {

                    value = arguments[index+1];
                    if( !( value.isEmpty() || _isTag(value) ) )
                    { index++; }

                }

                if( !( value.isEmpty() || _isTag(value) ) )
                {
                    _discardOrphans( ignoreWarnings );
                    iter.value().set_ = true;
                    iter.value().value_ = value;

                    // also find true option in group lists and set flag
                    for( auto&& groupIter = groups_.begin(); groupIter != groups_.end(); ++groupIter )
                    {
                        auto iter( _findTag( groupIter.value().options_, tagName ) );
                        if( iter != groupIter.value().options_.end() )
                        {
                            iter.value().set_ = true;
                            iter.value().value_ = value;
                        }
                    }

                }  else if( !ignoreWarnings ) {

                    QTextStream( stdout )
                        << "CommandLineParser::parse -"
                        << " expected argument of type " << iter.value().type_
                        << " after option " << iter.key().longName() << Qt::endl;

                }

                continue;

            }
        }

        // see if tag is an option
        if( _isTag( tagName ) )
        {

            if( !ignoreWarnings )
            {
                QTextStream( stdout )
                    << "CommandLineParser::parse - unrecognized option "
                    << tagName
                    << Qt::endl;
            }

            continue;

        }

        // add to orphans
        orphans_ << tagName;

    }

    return *this;

}

//________________________________________________
void CommandLineParser::clear()
{

    Debug::Throw( QStringLiteral("CommandLineParser::clear.\n") );

    applicationName_.clear();
    orphans_.clear();

    for( auto&& iter = groups_.begin(); iter != groups_.end(); ++iter )
    { iter.value().clear(); }

}

//_______________________________________________________
void CommandLineParser::_discardOrphans( bool ignoreWarnings )
{
    // print discarded orphans
    if( orphans_.isEmpty() ) return;

    if( !ignoreWarnings )
    {
        Debug::Throw(0) << "CommandLineParser::parse - following orphans are discarded: " << Qt::endl;
        for( const auto& orphan:orphans_ )
        { QTextStream( stdout ) << "  " << orphan << Qt::endl; }
    }

    orphans_.clear();
}

//_______________________________________________________
bool CommandLineParser::_isTag( const QString& tag ) const
{ return tag.startsWith( '-' ); }

//_______________________________________________________
CommandLineParser::Group::FlagMap CommandLineParser::_allFlags() const
{
    Group::FlagMap out;
    for( auto&& iter = groups_.cbegin(); iter != groups_.cend(); ++iter )
    { out.insert( iter.value().flags_ ); }

    return out;
}

//_______________________________________________________
CommandLineParser::Group::OptionMap CommandLineParser::_allOptions() const
{
    Group::OptionMap out;
    for( auto&& iter = groups_.cbegin(); iter != groups_.cend(); ++iter )
    { out.insert( iter.value().options_ ); }

    return out;
}

//_______________________________________________________
CommandLineParser::Group::FlagMap::iterator CommandLineParser::_findTag( Group::FlagMap& flags, const QString& tag ) const
{
    auto&& iter( flags.find( Tag( tag ) ) );
    if( iter == flags.end() )
    {
        const auto tags = flags.keys();
        auto tagIter = std::find_if( tags.cbegin(), tags.cend(), SameTagFTor( tag ) );
        if( tagIter != tags.cend() ) iter = flags.find( *tagIter );
    }

    return iter;
}

//_______________________________________________________
CommandLineParser::Group::FlagMap::const_iterator CommandLineParser::_findTag( const Group::FlagMap& flags, const QString& tag ) const
{
    auto iter = flags.find( Tag( tag ) );
    if( iter == flags.cend() )
    {
        const auto tags = flags.keys();
        auto tagIter = std::find_if( tags.cbegin(), tags.cend(), SameTagFTor( tag ) );
        if( tagIter != tags.cend() ) iter = flags.find( *tagIter );
    }

    return iter;
}

//_______________________________________________________
CommandLineParser::Group::OptionMap::iterator CommandLineParser::_findTag( Group::OptionMap& options, const QString& tag ) const
{
    auto iter( options.find( Tag( tag ) ) );
    if( iter == options.end() )
    {
        const Tag::List tags = options.keys();
        auto tagIter = std::find_if( tags.cbegin(), tags.cend(), SameTagFTor( tag ) );
        if( tagIter != tags.cend() ) iter = options.find( *tagIter );
    }

    return iter;
}

//_______________________________________________________
CommandLineParser::Group::OptionMap::const_iterator CommandLineParser::_findTag( const Group::OptionMap& options, const QString& tag ) const
{
    auto iter( options.find( Tag( tag ) ) );
    if( iter == options.cend() )
    {
        const Tag::List tags = options.keys();
        auto tagIter = std::find_if( tags.cbegin(), tags.cend(), SameTagFTor( tag ) );
        if( tagIter != tags.cend() ) iter = options.find( *tagIter );
    }

    return iter;
}
//_______________________________________________________
void CommandLineParser::Group::clear()
{
    // clear flags
    for( auto&& iter = flags_.begin(); iter != flags_.end(); ++iter )
    { iter.value().set_ = false; }

    // clear options
    for( auto&& iter = options_.begin(); iter != options_.end(); ++iter )
    {
        iter.value().set_ = false;
        iter.value().value_.clear();
    }
}

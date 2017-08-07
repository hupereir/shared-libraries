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
    Counter( "CommandLineParser" )
{
    // insert default groups
    groupNames_.append( applicationGroupName );
    groupNames_.append( serverGroupName );
    groupNames_.append( qtGroupName );

}

//________________________________________________________
void CommandLineParser::usage() const
{

    Debug::Throw( "CommandLineParser::usage.\n" );

    QTextStream stream( stdout );
    for( const auto& groupName:groupNames_ )
    {
        if( !groups_.contains( groupName ) ) continue;

        stream << endl;

        // header
        if( groups_.size() == 1 || groupName.isEmpty() ) stream << QObject::tr( "Options:" ) << endl;
        else stream << groupName << endl;

        // retrieve group
        const Group& group( groups_[groupName] );

        int maxFlagLength(0);
        if( !group.flags_.isEmpty() )
        {
            const QList<Tag> flagKeys( group.flags_.keys() );
            maxFlagLength = std::max_element( flagKeys.constBegin(), flagKeys.constEnd(), MinLengthFTor() )->size();
        }

        int maxOptionLength(0);
        int maxTypeLength(0);
        if( !group.options_.isEmpty() )
        {
            const QList<Tag> optionKeys( group.options_.keys() );
            const QList<Option> optionValues( group.options_.values() );
            maxOptionLength = std::max_element( optionKeys.constBegin(), optionKeys.constEnd(), MinLengthFTor() )->size();
            maxTypeLength = 1+ std::max_element( optionValues.constBegin(), optionValues.constEnd(), MinTypeLengthFTor() )->type_.size();
        }

        int maxLength = 1+ qMax( maxFlagLength, maxOptionLength );

        // print flags
        for( auto&& iter = group.flags_.constBegin(); iter != group.flags_.constEnd(); ++iter )
        {
            stream << "  ";
            stream << iter.key().toString().leftJustified( maxLength + maxTypeLength + 1 );
            stream << iter.value().helpText_ << endl;
        }

        // print options
        for( auto&& iter = group.options_.constBegin(); iter != group.options_.constEnd(); ++iter )
        {
            stream << "  ";
            stream << iter.key().toString().leftJustified( maxLength );
            stream << iter.value().type_.leftJustified( maxTypeLength + 1 );
            stream << iter.value().helpText_ << endl;
        }

    }

    stream
        << endl
        << QObject::tr( "Special tag \"-\" can be added to separate options from the last list of arguments\n (e.g. list of files to be opened)" )
        << endl;

    return;
}

//_______________________________________________________
CommandLineArguments CommandLineParser::arguments() const
{
    Debug::Throw( "CommandLineParser::arguments.\n" );

    CommandLineArguments out( { applicationName_ } );

    // add flags
    for( auto&& iter = groups_.constBegin(); iter != groups_.constEnd(); ++iter )
    {
        const Group& group = iter.value();

        for( auto&& iter = group.flags_.constBegin(); iter != group.flags_.constEnd(); ++iter )
        { if( iter.value().set_ ) out.append( iter.key().longName() ); }

        // add options
        for( auto&& iter = group.options_.constBegin(); iter != group.options_.constEnd(); ++iter )
        { if( iter.value().set_ && !iter.value().value_.isEmpty() ) out.append( QStringList( { iter.key().longName(), iter.value().value_ } ) ); }

    }

    // add orphans
    out.append( orphans_ );

    return out;

}

//_______________________________________________________
bool CommandLineParser::hasFlag( const QString& tag ) const
{
    const Group::FlagMap flags( _allFlags() );
    auto&& iter( _findTag( flags, tag ) );
    return iter != flags.end() && iter.value().set_;
}

//_______________________________________________________
bool CommandLineParser::hasOption( const QString& tag ) const
{
    const Group::OptionMap options( _allOptions() );
    auto&& iter( _findTag( options, tag ) );
    return iter != options.end() && iter.value().set_ && !iter.value().value_.isEmpty();
}

//_______________________________________________________
QString CommandLineParser::option( const QString& tag ) const
{
    const Group::OptionMap options( _allOptions() );
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
    Debug::Throw( "CommandLineParser::parse.\n" );

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

        if( tagName == "-" )
        {
            if( autoOrphan && !ignoreWarnings )
            {
                QTextStream(stdout) << "CommandLineParser::parse -"
                    << " option delimiter \"-\" appears multiple times "
                    << endl;
            }

            autoOrphan = true;
            continue;
        }

        // see if tag contains "=" string
        QString value;
        QStringList values( tagName.split( "=" ) );
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
                    << endl;
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
                            << endl;
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
                            << endl;
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
                        Group::OptionMap::iterator iter( _findTag( groupIter.value().options_, tagName ) );
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
                        << " after option " << iter.key().longName() << endl;

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
                    << endl;
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

    Debug::Throw( "CommandLineParser::clear.\n" );

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
        Debug::Throw(0) << "CommandLineParser::parse - following orphans are discarded: " << endl;
        for( const auto& orphan:orphans_ )
        { QTextStream( stdout ) << "  " << orphan << endl; }
    }

    orphans_.clear();
}

//_______________________________________________________
bool CommandLineParser::_isTag( const QString& tag ) const
{ return (!tag.isEmpty()) && tag.left(1) == "-"; }

//_______________________________________________________
CommandLineParser::Group::FlagMap CommandLineParser::_allFlags() const
{
    Group::FlagMap out;
    for( auto&& iter = groups_.constBegin(); iter != groups_.constEnd(); ++iter )
    { out.unite( iter.value().flags_ ); }

    return out;
}

//_______________________________________________________
CommandLineParser::Group::OptionMap CommandLineParser::_allOptions() const
{
    Group::OptionMap out;
    for( auto&& iter = groups_.constBegin(); iter != groups_.constEnd(); ++iter )
    { out.unite( iter.value().options_ ); }

    return out;
}

//_______________________________________________________
CommandLineParser::Group::FlagMap::iterator CommandLineParser::_findTag( Group::FlagMap& flags, const QString& tag ) const
{
    auto&& iter( flags.find( Tag( tag ) ) );
    if( iter == flags.end() )
    {
        const Tag::List tags = flags.keys();
        auto&& tagIter = std::find_if( tags.constBegin(), tags.constEnd(), SameTagFTor( tag ) );
        if( tagIter != tags.constEnd() ) iter = flags.find( *tagIter );
    }

    return iter;
}

//_______________________________________________________
CommandLineParser::Group::FlagMap::const_iterator CommandLineParser::_findTag( const Group::FlagMap& flags, const QString& tag ) const
{ return _findTag( const_cast<Group::FlagMap&>(flags), tag ); }

//_______________________________________________________
CommandLineParser::Group::OptionMap::iterator CommandLineParser::_findTag( Group::OptionMap& options, const QString& tag ) const
{
    auto iter( options.find( Tag( tag ) ) );
    if( iter == options.end() )
    {
        const Tag::List tags = options.keys();
        auto&& tagIter = std::find_if( tags.constBegin(), tags.constEnd(), SameTagFTor( tag ) );
        if( tagIter != tags.constEnd() ) iter = options.find( *tagIter );
    }

    return iter;
}

//_______________________________________________________
CommandLineParser::Group::OptionMap::const_iterator CommandLineParser::_findTag( const Group::OptionMap& options, const QString& tag ) const
{ return _findTag( const_cast<Group::OptionMap&>( options ), tag ); }

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

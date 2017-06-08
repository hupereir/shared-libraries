#ifndef CommandLineParser_h
#define CommandLineParser_h

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

#include "CommandLineArguments.h"
#include "Counter.h"
#include "Debug.h"

#include <QStringList>
#include <QHash>
#include <QMap>

//* parse command line argument.
class CommandLineParser: private Base::Counter<CommandLineParser>
{

    public:

    //* constructor
    explicit CommandLineParser( void );

    //*@name default group names
    //@{
    static const QString applicationGroupName;
    static const QString serverGroupName;
    static const QString qtGroupName;
    //@}

    //* used to register options/flags
    class Tag: private Base::Counter<Tag>
    {

        public:

        //* constructor
        explicit Tag( QString longName, QString shortName = QString() ):
            Counter( "CommandLineParser::Tag" ),
            longName_( longName ),
            shortName_( shortName )
        {}

        //* equal to operator
        bool operator == (const Tag& other ) const
        {
            if( shortName_.isNull() ) return longName_ == other.longName_ || longName_ == other.shortName_;
            else if( other.shortName_.isNull() ) return longName_ == other.longName_ || shortName_ == other.longName_;
            else return longName_ == other.longName_ && shortName_ == other.shortName_;
        }

        //* less than operator
        bool operator < (const Tag& other ) const
        {
            if( shortName_.isNull() )
            {
                if( other.shortName_.isNull() ) return longName_ < other.longName_;
                else return false;
            } else {
                if( other.shortName_.isNull() ) return true;
                else if( shortName_ != other.shortName_ ) return shortName_ < other.shortName_;
                else return  longName_ < other.longName_;
            }
        }

        //*@name accessors
        //@{

        //* long name
        const QString& longName( void ) const
        { return longName_; }

        //* short name
        const QString& shortName( void ) const
        { return shortName_; }

        //* convert to string
        QString toString( void ) const
        { return shortName_.isNull() ? longName_:(shortName_+", "+longName_); }

        //* size
        int size( void ) const
        { return toString().size(); }

        using List = QList<Tag>;

        private:

        QString longName_;
        QString shortName_;

    };

    //* accessors
    //@{

    //* print help
    void usage( void ) const;

    //* return 'rectified' arguments
    CommandLineArguments arguments( void ) const;

    //* application name
    QString applicationName( void ) const
    { return applicationName_; }

    //* return true if flag is on
    bool hasFlag( const QString& ) const;

    //* return true if option is set
    bool hasOption( const QString& ) const;

    //* return option associated to tag
    QString option( const QString& ) const;

    //* orphans
    /** list of command line arguments located at the end of the list and that do not match any option */
    const QStringList& orphans( void ) const
    { return orphans_; }

    //@}

    //* modifiers
    //@{

    //* register group
    void setGroup( QString groupName )
    {
        currentGroup_ = groupName;
        if( !groupNames_.contains( currentGroup_ ) )
        { groupNames_.append( currentGroup_ ); }
    }

    //* register option
    void registerOption( const QString& tag, const QString& type, QString helpText )
    { registerOption( Tag( tag ), type, helpText ); }

    //* register option
    void registerOption( const Tag&, const QString& type, const QString& helpText );

    //* register flag
    void registerFlag( const QString& tag, const QString& helpText )
    { registerFlag( Tag( tag ), helpText ); }

    //* register flag
    void registerFlag( const Tag&, const QString& );

    //* parse
    CommandLineParser& parse( const CommandLineArguments&, bool ignoreWarnings = true );

    //* clear
    void clear( void );

    //* orphans
    /** list of command line arguments located at the end of the list and that do not match any option */
    QStringList& orphans( void )
    { return orphans_; }

    //@}

    private:

    //* discard orphans
    void _discardOrphans( bool ignoreWarnings );

    //* returns true if string is a tag
    bool _isTag( const QString& ) const;

    //* flag class
    class Flag: private Base::Counter<Flag>
    {

        public:

        //* constructor
        explicit Flag( QString helpText = QString() ):
            Counter( "CommandLineParser::Flag" ),
            helpText_( helpText )
        {}

        //* help text
        QString helpText_;

        //* true if set
        bool set_ = false;

    };

    //* option class
    class Option: public Flag
    {

        public:

        //* constructor
        explicit Option( QString type = QString(), QString helpText = QString() ):
            Flag( helpText ),
            type_( type )
        {}

        //* type
        QString type_;

        //* value
        QString value_;

    };

    //* used to select tag of maximum length
    class MinLengthFTor
    {

        public:

        //* predicate
        bool operator () ( const Tag& first, const Tag& second )
        { return first.size() < second.size(); }

    };

    //* used to select tag of maximum length
    class MinTypeLengthFTor
    {

        public:

        //* predicate
        bool operator () ( const Option& first, const Option& second )
        { return first.type_.size() < second.type_.size(); }

    };

    //* used to find matching flag
    class SameTagFTor
    {

        public:

        //* constructor
        explicit SameTagFTor( const QString& tag ):
            tag_( tag )
            {}

        //* predicate
        bool operator() ( const Tag& tag ) const
        { return tag.shortName() == tag_ || tag.longName() == tag_; }

        private:

        //* tag
        QString tag_;

    };

    class Group
    {

        public:

        //* clear
        void clear( void );

        using Map = QHash< QString, Group>;

        //* flags
        using FlagMap = QMap<Tag, Flag>;

        //* flags
        FlagMap flags_;

        //* options
        using OptionMap = QMap<Tag, Option>;

        //* options
        OptionMap options_;

    };

    //* return all flags
    Group::FlagMap _allFlags( void ) const;

    //* return all options
    Group::OptionMap _allOptions( void ) const;

    //* find tag in list of flags
    Group::FlagMap::iterator _findTag( Group::FlagMap&, const QString& ) const;

    //* find tag in list of flags
    Group::FlagMap::const_iterator _findTag( const Group::FlagMap&, const QString& ) const;

    //* find tag in list of options
    Group::OptionMap::iterator _findTag( Group::OptionMap&, const QString& ) const;

    //* find tag in list of options
    Group::OptionMap::const_iterator _findTag( const Group::OptionMap&, const QString& ) const;

    //* group list
    Group::Map groups_;

    //* group list
    QStringList groupNames_;

    //* current group
    QString currentGroup_;

    //* application name
    QString applicationName_;

    //* orphan arguments
    QStringList orphans_;

};

#endif

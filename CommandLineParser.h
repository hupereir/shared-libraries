#ifndef CommandLineParser_h
#define CommandLineParser_h

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

#include "CommandLineArguments.h"
#include "Counter.h"
#include "Debug.h"

#include <QStringList>
#include <QMap>

//! parse command line argument.
class CommandLineParser: public Counter
{

    public:

    //! constructor
    CommandLineParser();

    //!@name default group names
    //@{
    static const QString applicationGroupName;
    static const QString serverGroupName;
    static const QString qtGroupName;
    //@}

    //! accessors
    //@{

    //! print help
    void usage( void ) const;

    //! return 'rectified' arguments
    CommandLineArguments arguments( void ) const;

    //! application name
    QString applicationName( void ) const
    { return applicationName_; }

    //! return true if flag is on
    bool hasFlag( QString ) const;

    //! return true if option is set
    bool hasOption( QString ) const;

    //! return option associated to tag
    QString option( QString ) const;

    //! orphans
    /*! list of command line arguments located at the end of the list and that do not match any option */
    const QStringList& orphans( void ) const
    { return orphans_; }

    //@}

    //! modifiers
    //@{

    //! register group
    void setGroup( QString groupName )
    {
        currentGroup_ = groupName;
        if( !groupNames_.contains( currentGroup_ ) )
        { groupNames_.append( currentGroup_ ); }
    }

    //! register option
    void registerOption( QString tag, QString type, QString helpText );

    //! register flag
    void registerFlag( QString tag, QString helpText );

    //! parse
    CommandLineParser& parse( const CommandLineArguments&, bool ignoreWarnings = true );

    //! clear
    void clear( void );

    //! orphans
    /*! list of command line arguments located at the end of the list and that do not match any option */
    QStringList& orphans( void )
    { return orphans_; }

    //@}

    private:

    //! discard orphans
    void _discardOrphans( bool ignoreWarnings );

    //! returns true if string is a tag
    bool _isTag( QString ) const;

    //! flag class
    class Flag: public Counter
    {

        public:

        //! constructor
        Flag( QString helpText = QString() ):
            Counter( "CommandLineParser::Flag" ),
            helpText_( helpText ),
            set_( false )
        {}

        //! destructor
        virtual ~Flag( void )
        {}

        //! help text
        QString helpText_;

        //! true if set
        bool set_;

    };

    //! option class
    class Option: public Flag
    {

        public:

        //! constructor
        Option( QString type = QString(), QString helpText = QString() ):
            Flag( helpText ),
            type_( type )
        {}

        //! type
        QString type_;

        //! value
        QString value_;

    };

    //! used to select tag of maximum length
    class MinLengthFTor
    {

        public:

        //! predicate
        bool operator () ( const QString& first, const QString& second )
        { return first.size() < second.size(); }

    };

    //! used to select tag of maximum length
    class MinTypeLengthFTor
    {

        public:

        //! predicate
        bool operator () ( const Option& first, const Option& second )
        { return first.type_.size() < second.type_.size(); }

    };

    class Group
    {

        public:

        //! clear
        void clear( void );

        typedef QHash< QString, Group> Map;

        //! flags
        typedef QMap<QString, Flag> FlagMap;

        //! flags
        FlagMap flags_;

        //! options
        typedef QMap<QString, Option> OptionMap;

        //! options
        OptionMap options_;

    };

    //! return all flags
    Group::FlagMap _allFlags( void ) const;

    //! return all options
    Group::OptionMap _allOptions( void ) const;

    //! group list
    Group::Map groups_;

    //! group list
    QStringList groupNames_;

    //! current group
    QString currentGroup_;

    //! application name
    QString applicationName_;

    //! orphan arguments
    QStringList orphans_;

};

#endif

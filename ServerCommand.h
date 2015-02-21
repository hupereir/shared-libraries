#ifndef ServerCommand_h
#define ServerCommand_h

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

#include "ApplicationId.h"
#include "CommandLineArguments.h"
#include "Counter.h"
#include "Debug.h"
#include "TimeStamp.h"
#include "XmlOption.h"

#include <QDomElement>
#include <QDomDocument>
#include <QString>
#include <QHash>
#include <QTextStream>

namespace Server
{

    //_________________________________________
    //* command to be send over the server
    class ServerCommand:public Counter
    {

        public:

        //* command type
        enum CommandType
        {
            None,
            Accepted,
            Alive,
            Denied,
            Abort,
            Identify,
            IdentifyServer,
            Killed,
            Raise,
            Request,
            Unlock,
            GeometryRequest,
            Geometry,
            Option
        };

        //* list
        using List = QList<ServerCommand>;

        //* constructor
        ServerCommand( const ApplicationId& id = ApplicationId(), const CommandType& command = None );

        //* constructor
        ServerCommand( const QDomElement& );

        //* equal-to operator
        bool operator == ( const ServerCommand& command ) const
        {
            return timeStamp() == command.timeStamp() &&
                id() == command.id() &&
                ServerCommand::command() == command.command();
        }

        //* less-than operator
        bool operator < ( const ServerCommand& command ) const
        {
            if( timeStamp() != command.timeStamp() ) return timeStamp() < command.timeStamp();
            if( id() != command.id() ) return id() < command.id();

            // to order command, place IDENTIFY_SERVER first
            if( ServerCommand::command() != command.command() ) return ( ServerCommand::command() == IdentifyServer || ServerCommand::command() < command.command() );
            return false;
        }

        //*@name accessors
        //@{

        //* dom element
        QDomElement domElement( QDomDocument& ) const;

        //* true if this command is a child of argument
        /*! this is needed to have commands appear in Tree view */
        bool isChild( const ServerCommand& command ) const
        { return timeStamp() >= command.timeStamp() && id() == command.id(); }

        //* time stamp
        const TimeStamp& timeStamp( void ) const
        { return timestamp_; }

        //* client id
        const int& clientId( void ) const
        { return clientId_; }

        //* application id
        const ApplicationId& id( void ) const
        { return id_; }

        //* command
        CommandType command( void ) const
        { return command_; }

        //* command name
        QString commandName( void ) const
        {
            _initializeCommandNames();
            return _commandNames()[ command() ];
        }

        //* arguments
        const CommandLineArguments& arguments( void ) const
        { return arguments_; }

        //* option
        const XmlOption& option( void ) const
        {
            Q_ASSERT( command() == ServerCommand::Option );
            return option_;
        }

        //@}

        //*@name modifiers
        //@{

        //* client id
        ServerCommand& setClientId( int value )
        {
            clientId_ = value;
            return *this;
        }

        //* application id
        void setId( const ApplicationId& id )
        { id_ = id; }

        //* command
        void setCommand( const CommandType& command )
        { command_ = command; }

        //* argument
        void setArguments( const CommandLineArguments& arguments )
        { arguments_ = arguments; }

        //* option
        void setXmlOption( const XmlOption& option )
        {
            Q_ASSERT( command() == ServerCommand::Option );
            option_ = option;
        }

        //@}

        private:

        //* command names
        using CommandMap = QHash<CommandType, QString >;

        //* command names
        static CommandMap& _commandNames( void );

        //* map command types to names
        void _initializeCommandNames( void ) const;

        //* time stamp
        TimeStamp timestamp_;

        //* client id
        int clientId_;

        //* application id
        ApplicationId id_;

        //* command
        CommandType command_;

        //* arguments
        CommandLineArguments arguments_;

        //* option
        XmlOption option_;

    };
};

#endif

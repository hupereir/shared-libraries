#ifndef ServerCommand_h
#define ServerCommand_h

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
#include "CppUtil.h"
#include "Debug.h"
#include "Operators.h"
#include "TimeStamp.h"
#include "XmlOption.h"
#include "base_server_export.h"


#include <QDataStream>
#include <QString>
#include <QHash>
#include <QTextStream>

namespace Server
{

    //_________________________________________
    //* command to be send over the server
    class BASE_SERVER_EXPORT ServerCommand final:private Base::Counter<ServerCommand>
    {

        public:

        //* command type
        enum class CommandType
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
        explicit ServerCommand( const ApplicationId& id = ApplicationId(), Server::ServerCommand::CommandType command = CommandType::None );

        //*@name accessors
        //@{

        //* true if this command is a child of argument
        /** this is needed to have commands appear in Tree view */
        bool isChild( const ServerCommand& command ) const
        { return timeStamp() >= command.timeStamp() && id() == command.id(); }

        //* time stamp
        const TimeStamp& timeStamp() const
        { return timestamp_; }

        //* client id
        quint32 clientId() const
        { return clientId_; }

        //* application id
        const ApplicationId& id() const
        { return id_; }

        //* command
        CommandType command() const
        { return command_; }

        //* command name
        QString commandName() const
        { return _commandNames()[ command() ]; }

        //* arguments
        const CommandLineArguments& arguments() const
        { return arguments_; }

        //* option
        const XmlOption& option() const
        { return option_; }

        //@}

        //*@name modifiers
        //@{

        //* client id
        void setClientId( quint32 value )
        { clientId_ = value; }

        //* application id
        void setId( const ApplicationId& id )
        { id_ = id; }

        //* command
        void setCommand( Server::ServerCommand::CommandType command )
        { command_ = command; }

        //* argument
        void setArguments( const CommandLineArguments& arguments )
        { arguments_ = arguments; }

        //* option
        void setXmlOption( const XmlOption& option )
        { option_ = option; }

        //@}

        private:

        //* command names
        using CommandMap = QHash<CommandType, QString>;

        //* command names
        static const CommandMap& _commandNames();

        //* time stamp
        TimeStamp timestamp_;

        //* client id
        quint32 clientId_ = 0;

        //* application id
        ApplicationId id_;

        //* command
        CommandType command_;

        //* arguments
        CommandLineArguments arguments_;

        //* option
        XmlOption option_;

        //*@name serializer
        //@{
        friend BASE_SERVER_EXPORT QDataStream& operator << (QDataStream&, const ServerCommand& );
        friend BASE_SERVER_EXPORT QDataStream& operator >> (QDataStream&, ServerCommand& );
        //@}
    };

    using ::qHash;

    //* equal-to operator
    inline bool operator == ( const ServerCommand& first, const ServerCommand& second )
    {
        return first.timeStamp() == second.timeStamp() &&
            first.id() == second.id() &&
            first.command() == second.command();
    }

}

#endif

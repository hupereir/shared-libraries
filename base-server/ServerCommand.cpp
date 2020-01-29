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


#include "ServerCommand.h"
#include "Debug.h"

#include <QStringList>

namespace Server
{

    //__________________________________________________________________
    const ServerCommand::CommandMap& ServerCommand::_commandNames()
    {
        static const CommandMap names( {
            { CommandType::None, "" },
            { CommandType::Accepted, QObject::tr( "Accepted" ) },
            { CommandType::Denied, QObject::tr( "Denied" ) },
            { CommandType::Abort, QObject::tr( "Abort" ) },
            { CommandType::Identify, QObject::tr( "Identify" ) },
            { CommandType::IdentifyServer, QObject::tr( "Server" ) },
            { CommandType::Killed, QObject::tr( "Killed" ) },
            { CommandType::Raise, QObject::tr( "Raise" ) },
            { CommandType::Alive, QObject::tr( "Alive" ) },
            { CommandType::Request, QObject::tr( "Request" ) },
            { CommandType::Unlock, QObject::tr( "Unlock" ) },
            { CommandType::GeometryRequest, QObject::tr(  "Request geometry" ) },
            { CommandType::Geometry, QObject::tr( "Send geometry" ) },
            { CommandType::Option, QObject::tr( "Option" ) }
        });
        return names;
    }

    //___________________________________________
    ServerCommand::ServerCommand( const ApplicationId& id, const CommandType& command ):
        Counter( "ServerCommand" ),
        timestamp_( TimeStamp::now() ),
        id_( id ),
        command_( command )
    { Debug::Throw( "ServerCommand::ServerCommand.\n" ); }

    //__________________________________________________
    QDataStream& operator << (QDataStream& stream, const ServerCommand& command )
    {
        static const quint32 version = 0;
        stream
            << version
            << command.timestamp_
            << command.clientId_
            << command.id_
            << quint32(command.command_)
            << command.arguments_.get()
            << command.option_.name()
            << command.option_;
        return stream;
    }

    //__________________________________________________
    QDataStream& operator >> (QDataStream& stream, ServerCommand& command )
    {
        quint32 version;
        stream >> version;
        if( version == 0 )
        {
            quint32 commandType;
            QStringList arguments;
            QString optionName;
            Option option;
            stream
                >> command.timestamp_
                >> command.clientId_
                >> command.id_
                >> commandType
                >> arguments
                >> optionName
                >> option;
            command.command_ = (ServerCommand::CommandType) commandType;
            command.arguments_ = CommandLineArguments( arguments );
            command.option_ = XmlOption( optionName, option );

        } else Debug::Throw(0) << "Unrecognized ServerCommand version: " << version << endl;

        return stream;
    }

}

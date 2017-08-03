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

#include "Debug.h"
#include "ServerCommand.h"

#include <QStringList>

namespace Server
{

    //__________________________________________________________________
    const ServerCommand::CommandMap& ServerCommand::_commandNames()
    {
        static const auto names = Base::makeT<CommandMap>( {
            { None, "" },
            { Accepted, QObject::tr( "Accepted" ) },
            { Denied, QObject::tr( "Denied" ) },
            { Abort, QObject::tr( "Abort" ) },
            { Identify, QObject::tr( "Identify" ) },
            { IdentifyServer, QObject::tr( "Server" ) },
            { Killed, QObject::tr( "Killed" ) },
            { Raise, QObject::tr( "Raise" ) },
            { Alive, QObject::tr( "Alive" ) },
            { Request, QObject::tr( "Request" ) },
            { Unlock, QObject::tr( "Unlock" ) },
            { GeometryRequest, QObject::tr(  "Request geometry" ) },
            { Geometry, QObject::tr( "Send geometry" ) },
            { Option, QObject::tr( "Option" ) }
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

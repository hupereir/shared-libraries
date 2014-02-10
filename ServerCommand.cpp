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

/*!
   \file    ServerCommand.cpp
   \brief   formated command for interprocess communication
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Debug.h"
#include "ServerCommand.h"
#include "ServerXmlDef.h"
#include "XmlCommandLineArguments.h"

#include <QStringList>

namespace Server
{

    //__________________________________________________________________
    ServerCommand::CommandMap& ServerCommand::_commandNames( void )
    {
        static CommandMap names;
        return names;
    }

    //_________________________________________
    void ServerCommand::_initializeCommandNames( void ) const
    {
        if( !_commandNames().empty() ) return;
        _commandNames().insert( None, "" );
        _commandNames().insert( Accepted, QObject::tr( "Accepted" ) );
        _commandNames().insert( Denied, QObject::tr( "Denied" ) );
        _commandNames().insert( Abort, QObject::tr( "Abort" ) );
        _commandNames().insert( Identify, QObject::tr( "Identify" ) );
        _commandNames().insert( IdentifyServer, QObject::tr( "Server" ) );
        _commandNames().insert( Killed, QObject::tr( "Killed" ) );
        _commandNames().insert( Raise, QObject::tr( "Raise" ) );
        _commandNames().insert( Alive, QObject::tr( "Alive" ) );
        _commandNames().insert( Request, QObject::tr( "Request" ) );
        _commandNames().insert( Unlock, QObject::tr( "Unlock" ) );
        _commandNames().insert( GeometryRequest, QObject::tr(  "Request geometry" ) );
        _commandNames().insert( Geometry, QObject::tr( "Send geometry" ) );
        _commandNames().insert( Option, QObject::tr( "Option" ) );
    }

    //___________________________________________
    ServerCommand::ServerCommand( const ApplicationId& id, const CommandType& command ):
        Counter( "ServerCommand" ),
        timestamp_( TimeStamp::now() ),
        clientId_( 0 ),
        id_( id ),
        command_( command ),
        option_( "", ::Option() )
    { Debug::Throw( "ServerCommand::ServerCommand.\n" ); }

    //___________________________________________
    ServerCommand::ServerCommand( const QDomElement& element ):
        Counter( "ServerCommand" ),
        timestamp_( TimeStamp::now() ),
        clientId_( 0 ),
        command_( None ),
        option_( "", ::Option() )
    {

        Debug::Throw( "ServerCommand::ServerCommand (dom).\n" );

        // parse attributes
        QDomNamedNodeMap attributes( element.attributes() );
        for( unsigned int i=0; i<attributes.length(); i++ )
        {
            QDomAttr attribute( attributes.item( i ).toAttr() );
            if( attribute.isNull() ) continue;
            if( attribute.name() == Xml::TYPE ) setCommand( (CommandType) attribute.value().toUInt() );
            else Debug::Throw(0) << "ServerCommand::ServerCommand - unrecognized attribute: " << attribute.name() << endl;
        }

        // loop over children
        // parse children elements
        for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
        {
            QDomElement child_element = child_node.toElement();
            if( child_element.isNull() ) continue;
            QString tagName( child_element.tagName() );
            if( tagName == Xml::ID ) setId( ApplicationId( child_element ) );
            else if( tagName == Xml::ARGUMENTS ) setArguments( XmlCommandLineArguments( child_element ) );
            else if( tagName == Base::Xml::OPTION )
            {
                Q_ASSERT( command() == ServerCommand::Option );
                setXmlOption( XmlOption( child_element ) );
            }
        }

    }

    //__________________________________________________
    QDomElement ServerCommand::domElement( QDomDocument& document ) const
    {

        Debug::Throw( "ServerCommand::domElement.\n" );
        QDomElement out( document.createElement( Xml::COMMAND ) );

        // type
        out.setAttribute( Xml::TYPE, QString::number( command() ) );

        // id
        out.appendChild( id().domElement( document ) );

        // arguments
        if( !arguments().isEmpty() ) out.appendChild( XmlCommandLineArguments(arguments()).domElement( Xml::ARGUMENTS, document ) );
        if( command() == ServerCommand::Option && !option_.name().isEmpty() )
        { out.appendChild( option_.domElement( document ) ); }

        return out;

    }

}

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

#include "Client.h"
#include "Client.moc"
#include "Debug.h"
#include "IOString.h"
#include "ServerXmlDef.h"
#include "XmlDocument.h"

#include <QTextStream>

namespace Server
{
    //_______________________________________________________
    unsigned int& Client::_counter( void )
    {
        static unsigned int counter( 0 );
        return counter;
    }

    //_______________________________________________________
    Client::Client( QObject* parent, QTcpSocket* socket ):
        QObject( parent ),
        Counter( "Client" ),
        id_( _counter()++ ),
        socket_( socket )
    {
        Debug::Throw( "Client::Client.\n" );
        Q_CHECK_PTR( socket );
        connect( socket_, SIGNAL(connected()), SLOT(_sendCommands()) );
        connect( socket_, SIGNAL(readyRead()), SLOT(_readMessage()) );
    }

    //_______________________________________________________
    Client::~Client( void )
    { Debug::Throw( "Client::~Client.\n" ); }

    //_______________________________________________________
    bool Client::sendCommand( const ServerCommand& command )
    {

        commands_ << command;
        if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendCommands();
        return true;

    }

    //_______________________________________________________
    void Client::_sendCommands( void )
    {

        if( commands_.empty() ) return;

        QDomDocument document;
        QDomElement top = document.appendChild( document.createElement( Xml::Transmission ) ).toElement();
        while( commands_.size() && socket().state() == QAbstractSocket::ConnectedState )
        {
            top.appendChild( commands_.front().domElement( document ) );
            commands_.removeFirst();
        }

        QTextStream os( &socket() );
        os << document.toString() << endl;

    }

    //_______________________________________________________
    bool Client::_readMessage( void )
    {

        // read everything from socket and store as message
        IOString message( socket() );
        if( message.isEmpty() ) return false;

        // add to buffer
        buffer_.append( message );

        // parse buffer
        static const QString beginTag = (QStringList() << "<" << Xml::Transmission << ">" ).join("");
        static const QString endTag = (QStringList() << "</" << Xml::Transmission << ">" ).join("");

        while(1)
        {

            // get first tag
            int begin_position( _messageBuffer().text().indexOf( beginTag, _messageBuffer().position() ) );
            if( begin_position < 0 ) break;

            // get end tag
            int end_position( _messageBuffer().text().indexOf( endTag, begin_position+beginTag.size() ) );
            if( end_position < 0 ) break;

            // create QDomDocument
            QString local( _messageBuffer().text().mid( begin_position, end_position+endTag.size()-begin_position ) );

            // create document
            XmlDocument document;
            if( !document.setContent( local ) )
            {

                Debug::Throw() << document.error() << endl;

            } else {

                // parse document
                QDomElement docElement = document.documentElement();
                Q_ASSERT( docElement.tagName() == Xml::Transmission );

                for(QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() )
                {
                    QDomElement element = node.toElement();
                    if( element.isNull() ) continue;
                    if( element.tagName() == Xml::Command ) {

                        ServerCommand command( element );
                        emit commandAvailable( command.setClientId( id() ) );

                    } else { Debug::Throw(0) << "ServerCommand::_readMessage - unrecognized tagname: " << element.tagName() << endl; }

                }

            }

            // flush buffer
            _messageBuffer().flush( end_position+endTag.size() );

        }

        return true;

    }

}

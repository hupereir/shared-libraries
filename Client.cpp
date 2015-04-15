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
#include "Debug.h"
#include "ServerXmlDef.h"
#include "XmlDocument.h"

#include <QTextStream>

namespace Server
{
    //_______________________________________________________
    int& Client::_counter( void )
    {
        static int counter( 0 );
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
        if( socket_->state() ==  QAbstractSocket::ConnectedState ) _sendCommands();
        return true;

    }

    //_______________________________________________________
    void Client::_sendCommands( void )
    {

        if( commands_.empty() ) return;

        QDomDocument document;
        QDomElement top = document.appendChild( document.createElement( Xml::Transmission ) ).toElement();
        while( commands_.size() && socket_->state() == QAbstractSocket::ConnectedState )
        {
            top.appendChild( commands_.front().domElement( document ) );
            commands_.removeFirst();
        }

        socket_->write( document.toByteArray() );

    }

    //_______________________________________________________
    bool Client::_readMessage( void )
    {

        // add to buffer
        if( socket_->bytesAvailable() ) buffer_.append( QString::fromUtf8( socket_->readAll() ) );
        else return false;

        // parse buffer
        static const QString beginTag = QString( "<%1>" ).arg(Xml::Transmission);
        static const QString endTag = QString( "</%1>" ).arg(Xml::Transmission);

        forever
        {

            // get first tag
            int beginPosition( buffer_.text().indexOf( beginTag, buffer_.position() ) );
            if( beginPosition < 0 ) break;

            // get end tag
            int endPosition( buffer_.text().indexOf( endTag, beginPosition+beginTag.size() ) );
            if( endPosition < 0 ) break;

            // create QDomDocument
            QString local( buffer_.text().mid( beginPosition, endPosition+endTag.size()-beginPosition ) );

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
            buffer_.flush( endPosition+endTag.size() );

        }

        return true;

    }

}

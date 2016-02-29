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

#include <QDataStream>

namespace Server
{

    static const qint32 CommandType = 1;

    //_______________________________________________________
    quint32& Client::_counter( void )
    {
        static quint32 counter( 0 );
        return counter;
    }

    //_______________________________________________________
    Client::Client( QObject* parent, QTcpSocket* socket ):
        BaseSocketInterface( parent, socket ),
        Counter( "Server::Counter" ),
        id_( _counter()++ )
    { connect( this, SIGNAL(bufferReceived(qint32,QByteArray)), SLOT(_parseBuffer(qint32,QByteArray)) ); }

    //_______________________________________________________
    void Client::sendCommand( const ServerCommand& command )
    {
        QByteArray array;
        QDataStream stream( &array, QIODevice::WriteOnly );
        stream << command;
        sendBuffer( CommandType, array );
    }

    //_______________________________________________________
    void Client::_parseBuffer( qint32 bufferType, QByteArray buffer )
    {
        if( bufferType == CommandType )
        {
            ServerCommand command;
            QDataStream stream( &buffer, QIODevice::ReadOnly );
            stream >> command;
            command.setClientId( id() );
            emit commandAvailable( command );
        }

    }

}

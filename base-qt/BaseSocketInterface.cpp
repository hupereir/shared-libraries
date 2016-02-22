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

#include "BaseSocketInterface.h"
#include "Debug.h"

#include <QtEndian>

//_______________________________________________________
BaseSocketInterface::BaseSocketInterface( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "BaseSocketInterface" ),
  socket_( socket )
{
    Debug::Throw( "BaseSocketInterface::BaseSocketInterface.\n" );
    Q_CHECK_PTR( socket );
    connect( socket_, SIGNAL(connected()), SLOT(_sendPendingBuffers()) );
    connect( socket_, SIGNAL(readyRead()), SLOT(_read()) );
}

//_______________________________________________________
void BaseSocketInterface::sendBuffer( qint32 type, const QByteArray& buffer )
{
    if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendBuffer( type, buffer );
    else pendingBuffers_.append( BufferPair( type, buffer ) );
}

//_______________________________________________________
void BaseSocketInterface::_sendPendingBuffers( void )
{

    foreach( auto bufferPair, pendingBuffers_ )
    { _sendBuffer( bufferPair.first, bufferPair.second ); }

    pendingBuffers_.clear();

}

//_______________________________________________________
void BaseSocketInterface::_sendBuffer( qint32 type, const QByteArray& buffer )
{
    quint64 bufferSize( qToBigEndian( buffer.size() ) );
    Debug::Throw(0) << "BaseSocketInterface::_sendBuffer - type: " << type << " size: " << bufferSize << endl;
    socket_->write( reinterpret_cast<const char*>( &type ), sizeof( qint32 ) );
    socket_->write( reinterpret_cast<const char*>( &bufferSize ), sizeof( quint64 ) );
    socket_->write( buffer );
}

//_______________________________________________________
void BaseSocketInterface::_read( void )
{

    if( !socket_->bytesAvailable() ) return;

    Debug::Throw(0) << "BaseSocketInterface::_read - bytes: " << socket_->bytesAvailable() << endl;
    forever
    {

        // buffer type
        if( bufferType_ < 0 )
        {
            if( socket_->bytesAvailable() >= int(sizeof( qint32 )) ) socket_->read( reinterpret_cast<char*>( &bufferType_ ), sizeof( qint32 ) );
            else break;

            Debug::Throw(0)
                << "BaseSocketInterface::_read -"
                << " bufferType: " << bufferType_
                << " bytes: " << socket_->bytesAvailable()
                << endl;

        }

        if( bufferSize_ == 0 )
        {
            quint64 bufferSize;
            if( socket_->bytesAvailable() >= int(sizeof( quint64 )) ) socket_->read( reinterpret_cast<char*>( &bufferSize ), sizeof( quint64 ) );
            else break;

            bufferSize_ = qFromBigEndian( bufferSize );
            Debug::Throw(0)
                << "BaseSocketInterface::_read -"
                << " bufferSize: " << bufferSize_
                << " bytes: " << socket_->bytesAvailable()
                << endl;
        }

        if( bufferSize_ == 0 ) break;
        if( quint64(socket_->bytesAvailable()) >= bufferSize_ )
        {

            const QByteArray array = socket_->read( bufferSize_ );
            const qint32 bufferType = bufferType_;
            bufferSize_ = 0;
            bufferType_ = -1;
            emit bufferReceived( bufferType, array );

            Debug::Throw(0)
                << "BaseSocketInterface::_read -"
                << " array size: " << array.size()
                << " bytes: " << socket_->bytesAvailable()
                << endl;

        } else break;
    }

}

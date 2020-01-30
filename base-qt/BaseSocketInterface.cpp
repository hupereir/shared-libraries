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

static const int debugLevel = 1;

//_______________________________________________________
BaseSocketInterface::BaseSocketInterface( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  socket_( socket )
{
    Debug::Throw( "BaseSocketInterface::BaseSocketInterface.\n" );
    if( !socket_ ) socket_ = new QTcpSocket( this );
    connect( socket_, &QAbstractSocket::connected, this, &BaseSocketInterface::_sendPendingBuffers );
    connect( socket_, &QIODevice::readyRead, this, &BaseSocketInterface::_read );
}

//_______________________________________________________
void BaseSocketInterface::sendBuffer( qint32 type, const QByteArray& buffer )
{
    if( socket().state() ==  QAbstractSocket::ConnectedState ) _sendBuffer( type, buffer );
    else pendingBuffers_.append( BufferPair( type, buffer ) );
}

//_______________________________________________________
void BaseSocketInterface::_sendPendingBuffers()
{

    for( const auto& bufferPair:pendingBuffers_ )
    { _sendBuffer( bufferPair.first, bufferPair.second ); }

    pendingBuffers_.clear();

}

//_______________________________________________________
void BaseSocketInterface::_sendBuffer( qint32 type, const QByteArray& buffer )
{
    buffersize_t bufferSize( buffer.size() );
    Debug::Throw(debugLevel) << "BaseSocketInterface::_sendBuffer - type: " << type << " size: " << bufferSize << endl;
    socket_->write( reinterpret_cast<const char*>( &type ), sizeof( buffertype_t ) );
    socket_->write( reinterpret_cast<const char*>( &bufferSize ), sizeof( buffersize_t ) );
    socket_->write( buffer );
}

//_______________________________________________________
void BaseSocketInterface::_read()
{

    if( !socket_->bytesAvailable() ) return;

    Debug::Throw(debugLevel) << "BaseSocketInterface::_read - bytes: " << socket_->bytesAvailable() << endl;
    forever
    {

        // buffer type
        if( bufferType_ < 0 )
        {
            if( socket_->bytesAvailable() >= int(sizeof( buffertype_t )) ) socket_->read( reinterpret_cast<char*>( &bufferType_ ), sizeof( buffertype_t ) );
            else break;

            Debug::Throw(debugLevel)
                << "BaseSocketInterface::_read -"
                << " bufferType: " << bufferType_
                << " bytes: " << socket_->bytesAvailable()
                << endl;

        }

        if( bufferSize_ == 0 )
        {
            if( socket_->bytesAvailable() >= int(sizeof( buffersize_t )) ) socket_->read( reinterpret_cast<char*>( &bufferSize_ ), sizeof( buffersize_t ) );
            else break;

            Debug::Throw(debugLevel)
                << "BaseSocketInterface::_read -"
                << " bufferSize: " << bufferSize_
                << " bytes: " << socket_->bytesAvailable()
                << endl;
        }

        if( bufferSize_ == 0 ) break;
        if( quint64(socket_->bytesAvailable()) >= bufferSize_ )
        {

            const QByteArray array = socket_->read( bufferSize_ );
            const buffertype_t bufferType = bufferType_;
            bufferSize_ = 0;
            bufferType_ = -1;
            emit bufferReceived( bufferType, array );

            Debug::Throw(debugLevel)
                << "BaseSocketInterface::_read -"
                << " array size: " << array.size()
                << " bytes: " << socket_->bytesAvailable()
                << endl;

        } else break;
    }

}

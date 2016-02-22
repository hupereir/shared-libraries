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

//_______________________________________________________
BaseSocketInterface::BaseSocketInterface( QObject* parent, QTcpSocket* socket ):
  QObject( parent ),
  Counter( "BaseSocketInterface" ),
  socket_( socket )
{
    Debug::Throw( "BaseSocketInterface::BaseSocketInterface.\n" );
    Q_CHECK_PTR( socket );
    connect( socket_, SIGNAL(readyRead()), SLOT(_read()) );
}

//_______________________________________________________
void BaseSocketInterface::sendBuffer( qint32 type, const QByteArray& buffer )
{
    quint64 bufferSize( buffer.size() );
    socket_->write( reinterpret_cast<const char*>( &type ), sizeof( qint32 ) );
    socket_->write( reinterpret_cast<const char*>( &bufferSize ), sizeof( quint64 ) );
    socket_->write( buffer );
}

//_______________________________________________________
void BaseSocketInterface::_read( void )
{

    if( !socket_->bytesAvailable() ) return;

    Debug::Throw(0) << "BaseSocketInterface::_read - bytes available: " << socket_->bytesAvailable() << endl;
    forever
    {

        // buffer type
        if( bufferType_ < 0 )
        {
            if( socket_->bytesAvailable() >= int(sizeof( qint32 )) )
            {
                Debug::Throw(0, "BaseSocketInterface::_read - reading buffer type\n" );
                socket_->read( reinterpret_cast<char*>( &bufferType_ ), sizeof( qint32 ) );
                Debug::Throw(0)
                    << "BaseSocketInterface::_read -"
                    << " buffer type: " << bufferType_
                    << " bytes available: " << socket_->bytesAvailable()
                    << endl;
           } else break;
        }

        if( bufferSize_ == 0 )
        {
            if( socket_->bytesAvailable() >= int(sizeof( quint64 )) )
            {
                Debug::Throw(0, "BaseSocketInterface::_read - reading buffer size\n" );
                socket_->read( reinterpret_cast<char*>( &bufferSize_ ), sizeof( quint64 ) );
                Debug::Throw(0)
                    << "BaseSocketInterface::_read -"
                    << " buffer size: " << bufferSize_
                    << " bytes available: " << socket_->bytesAvailable()
                    << endl;
            } else break;
        }

        if( bufferSize_ == 0 ) break;
        if( quint64(socket_->bytesAvailable()) >= bufferSize_ )
        {

            Debug::Throw(0, "BaseSocketInterface::_read - reading buffer\n" );
            const QByteArray array = socket_->read( bufferSize_ );
            emit bufferReceived( bufferType_, array );

            bufferSize_ = 0;
            bufferType_ = -1;
            Debug::Throw(0)
                << "BaseSocketInterface::_read -"
                << " buffer size: " << array.size()
                << " bytes available: " << socket_->bytesAvailable()
                << endl;

        } else break;
    }

}

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

#include "SshBaseSocket.h"
#include "Debug.h"

#include <QElapsedTimer>

#if HAVE_SSH
#include <libssh2.h>
#endif

namespace Ssh
{

    //_______________________________________________________________________
    BaseSocket::BaseSocket( QObject* parent ):
        QIODevice( parent ),
        Counter( "Ssh::BaseSocket" )
    { buffer_.resize( maxSize_ ); }

    //_______________________________________________________________________
    BaseSocket::~BaseSocket( void )
    { close(); }

    //_______________________________________________________________________
    bool BaseSocket::atEnd( void ) const
    {

        // check channel
        if( !isConnected() ) return true;

        #if HAVE_SSH
        return libssh2_channel_eof( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
        #else
        return true;
        #endif
    }

    //_______________________________________________________________________
    void BaseSocket::close( void )
    {
        Debug::Throw( "Ssh::BaseSocket:close.\n" );

        // stop timer
        if( timer_.isActive() ) timer_.stop();

        #if HAVE_SSH
        // close channel
        if( isConnected() )
        {
            libssh2_channel_free( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
            channel_ = nullptr;
        }
        #endif

    }

    //_______________________________________________________________________
    qint64 BaseSocket::readData( char* data, qint64 maxSize )
    {

        Debug::Throw() << "Ssh::BaseSocket::readData - length: " << maxSize << endl;
        if( !( openMode() & QIODevice::ReadOnly ) )
        {
            setErrorString( tr( "Socket is writeOnly" ) );
            return -1;
        }

        if( bytesAvailable_ <= 0 ) return bytesAvailable_;

        const qint64 bytesRead = qMin( maxSize, bytesAvailable_ );
        memcpy( data, buffer_.data(), bytesRead );
        buffer_ = buffer_.mid( bytesRead );
        buffer_.resize( maxSize_ );
        bytesAvailable_ -= bytesRead;
        return bytesRead;

    }

    //_______________________________________________________________________
    qint64 BaseSocket::writeData( const char* data, qint64 maxSize )
    {

        Debug::Throw() << "Ssh::BaseSocket::writeData - size: " << maxSize << endl;

        if( !(openMode() & QIODevice::WriteOnly) )
        {
            setErrorString( tr( "Socket is readOnly" ) );
            return -1;
        }

        if( !isConnected() ) return -1;
        #if HAVE_SSH

        qint64 bytesWritten = 0;
        LIBSSH2_CHANNEL* channel = reinterpret_cast<LIBSSH2_CHANNEL*>(channel_);
        while( bytesWritten < maxSize )
        {

            const qint64 i = libssh2_channel_write( channel, data + bytesWritten, maxSize - bytesWritten );
            if( i >= 0 ) bytesWritten += i;
            else if( i != LIBSSH2_ERROR_EAGAIN )
            {
                setErrorString( tr( "invalid write: %1" ).arg( i ) );
                return -1;
            }

        }

        return bytesWritten;

        #else

        setErrorString( "invalid channel" );
        return -1;

        #endif

    }

    //_______________________________________________________________________
    void BaseSocket::timerEvent( QTimerEvent* event )
    {

        // check timer id
        if( event->timerId() != timer_.timerId() ) return QIODevice::timerEvent( event );

        #if HAVE_SSH

        if( isConnected() ) _tryRead();
        return;

        #else

        timer_.stop();
        bytesAvailable_ = -1;
        setErrorString( "no ssh" );
        return;

        #endif

    }

    //_______________________________________________________________________
    void BaseSocket::_setChannel( void* channel, QIODevice::OpenMode openMode )
    {

        // close existing channel if any
        if( isConnected() ) close();

        // stop timer
        if( timer_.isActive() ) timer_.stop();

        // assign open mode
        setOpenMode( openMode );

        // assign new channel
        channel_ = channel;

        if( isConnected() )
        {

            // start timer
            if( openMode&QIODevice::ReadOnly ) timer_.start( latency_, this );

            // emit signal
            emit connected();

        }

    }

    //_______________________________________________________________________
    bool BaseSocket::_tryRead( void )
    {

        if( !isConnected() ) return false;

        #if HAVE_SSH

        // read from channel
        LIBSSH2_CHANNEL* channel = reinterpret_cast<LIBSSH2_CHANNEL*>(channel_);
        qint64 length =  libssh2_channel_read( channel, buffer_.data()+bytesAvailable_, maxSize_-bytesAvailable_ );
        if( length == LIBSSH2_ERROR_EAGAIN ) return false ;
        else if( length < 0 )
        {

            setErrorString( tr( "invalid read: %1" ).arg( length ) );
            timer_.stop();
            bytesAvailable_ = -1;

            return false;

        } else {

            bytesAvailable_ += length;
            emit readyRead();

        }

        // check at end
        if( atEnd() )
        {
            timer_.stop();
            setErrorString( "channel closed" );
            emit readChannelFinished();
        }

        return true;

        #else
        return false;
        #endif

    }

}

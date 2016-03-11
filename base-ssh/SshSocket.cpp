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

#include "SshSocket.h"
#include "Debug.h"

#if HAVE_SSH
#include <libssh2.h>
#endif

namespace Ssh
{

    //_______________________________________________________________________
    Socket::Socket( QObject* parent ):
        QIODevice( parent ),
        Counter( "Ssh::Socket" )
    { buffer_.resize( maxSize_ ); }

    //_______________________________________________________________________
    Socket::~Socket( void )
    { close(); }

    //_______________________________________________________________________
    void Socket::connectToHost( void* session, const QString& host, quint16 port )
    {

        Debug::Throw() << "Ssh::Socket::connectToHost - " << host << ":" << port << endl;

        // disconnect
        if( channel_ ) close();

        // store session, host and port
        session_ = session;
        host_ = host;
        port_ = port;

        // make sure timer is running
        if( !timer_.isActive() ) timer_.start( latency_, this );

    }

    //_______________________________________________________________________
    void Socket::waitForConnected( void )
    { forever { if( _tryConnect() ) return; } }

    //_______________________________________________________________________
    bool Socket::atEnd( void ) const
    {

        // check channel
        if( !channel_ ) return true;

        #if HAVE_SSH
        return libssh2_channel_eof( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
        #else
        return true;
        #endif
    }

    //_______________________________________________________________________
    void Socket::close( void )
    {
        Debug::Throw( "Ssh::Socket:close.\n" );

        // stop timer
        if( timer_.isActive() ) timer_.stop();

        #if HAVE_SSH
        // close channel
        if( channel_ )
        {
            libssh2_channel_free( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
            channel_ = nullptr;
        }
        #endif

    }

    //_______________________________________________________________________
    qint64 Socket::readData( char* data, qint64 maxSize )
    {

        Debug::Throw() << "Ssh::Socket::readData - length: " << maxSize << endl;
        if( bytesAvailable_ <= 0 ) return bytesAvailable_;

        const qint64 bytesRead = qMin( maxSize, bytesAvailable_ );
        memcpy( data, buffer_.data(), bytesRead );
        buffer_ = buffer_.mid( bytesRead );
        buffer_.resize( maxSize_ );
        bytesAvailable_ -= bytesRead;
        return bytesRead;

    }

    //_______________________________________________________________________
    qint64 Socket::writeData( const char* data, qint64 maxSize )
    {

        Debug::Throw() << "Ssh::Socket::writeData - size: " << maxSize << endl;

        if( !channel_ ) return -1;
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
    void Socket::timerEvent( QTimerEvent* event )
    {

        // check timer id
        if( event->timerId() != timer_.timerId() ) return QIODevice::timerEvent( event );

        #if HAVE_SSH

        if( channel_ || _tryConnect() ) _tryRead();
        return;

        #else

        timer_.stop();
        bytesAvailable_ = -1;
        setErrorString( "no ssh" );
        return;

        #endif

    }

    //_______________________________________________________________________
    bool Socket::_tryConnect( void )
    {

        if( channel_ ) return true;

        #if HAVE_SSH
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );
        if( (channel_ = libssh2_channel_direct_tcpip( session, qPrintable( host_ ), port_ ) ) )
        {

            setOpenMode( ReadWrite );
            emit connected();
            return true;

        } else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN ) {

            timer_.stop();
            char *errMsg(nullptr);
            libssh2_session_last_error(session, &errMsg, NULL, 0);
            setErrorString( tr( "error getting direct tcp channel: %1" ).arg( errMsg ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;

        }

        return false;

        #else
        return true;
        #endif

    }

    //_______________________________________________________________________
    bool Socket::_tryRead( void )
    {

        if( !channel_ ) return false;

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
        return falsee;
        #endif

    }

}

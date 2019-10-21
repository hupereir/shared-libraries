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

#include <QElapsedTimer>

#if WITH_SSH
#include <libssh/libssh.h>
#endif

namespace Ssh
{

    //* max buffer size
    static const qint64 maxBufferSize = 1<<16;

    //_______________________________________________________________________
    Socket::Socket( QObject* parent ):
        QIODevice( parent ),
        Counter( "Ssh::Socket" )
    {
        buffer_.resize( maxBufferSize );
        setOpenMode(QIODevice::ReadWrite);
    }

    //_______________________________________________________________________
    Socket::~Socket()
    { close(); }

    //_______________________________________________________________________
    bool Socket::atEnd() const
    {

        // check channel
        if( !isConnected() ) return true;

        #if WITH_SSH
        return ssh_channel_is_eof( static_cast<ssh_channel>(channel_.get()) );
        #else
        return true;
        #endif
    }

    //_______________________________________________________________________
    void Socket::connectToHost( void* session, const QString& host, quint16 port )
    {

        Debug::Throw() << "Ssh::Socket::connectToHost - " << host << ":" << port << endl;

        // store session, host and port
        session_ = session;
        host_ = host;
        port_ = port;

        // make sure timer is running
        if( timer_.isActive() ) timer_.stop();
        if( !_tryConnect() ) timer_.start( latency_, this );

    }

    //_______________________________________________________________________
    bool Socket::waitForConnected( int msecs )
    {

        // do nothing if already connected
        if( isConnected() ) return true;

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        { if( _tryConnect() ) return true; }

        return false;

    }

    //_______________________________________________________________________
    void Socket::close()
    {
        Debug::Throw( "Ssh::Socket:close.\n" );

        // stop timer
        if( timer_.isActive() ) timer_.stop();
        channel_.reset();

    }

    //_______________________________________________________________________
    qint64 Socket::readData( char* data, qint64 maxSize )
    {

        if( bytesAvailable_ <= 0 ) return bytesAvailable_;

        const qint64 bytesRead = qMin( maxSize, bytesAvailable_ );
        memcpy( data, buffer_.data(), bytesRead );
        buffer_.remove( 0, bytesRead );
        buffer_.resize( maxBufferSize );
        bytesAvailable_ -= bytesRead;

        return bytesRead;

    }

    //_______________________________________________________________________
    qint64 Socket::writeData( const char* data, qint64 maxSize )
    {

        if( !isConnected() ) return -1;
        #if WITH_SSH

        qint64 bytesWritten = 0;
        auto channel = static_cast<ssh_channel>(channel_.get());
        while( bytesWritten < maxSize )
        {

            const qint64 i = ssh_channel_write( channel, data + bytesWritten, maxSize - bytesWritten );
            if( i >= 0 ) bytesWritten += i;
            else if( i == SSH_ERROR )
            {
                setErrorString( tr( "invalid write" ) );
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

        #if WITH_SSH

        if( !isConnected() ) _tryConnect();
        else _tryRead();
        return;

        #else

        timer_.stop();
        bytesAvailable_ = -1;
        setErrorString( "no ssh" );
        return;

        #endif

    }

    //_______________________________________________________________________
    bool Socket::_tryConnect()
    {

        if( isConnected() ) return true;

        #if WITH_SSH

        auto session( static_cast<ssh_session>(session_) );
        auto channel( static_cast<ssh_channel>(channel_.get()) );
        if( !channel )
        {
            channel = ssh_channel_new(session);
            if( channel ) channel_.reset( channel );
            else
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }
        }

        auto result = ssh_channel_open_forward( channel, qPrintable( host_ ), port_, qPrintable( localHost_ ), localPort_ );
        if( result == SSH_OK )
        {

            connected_ = true;
            emit connected();
            return true;

        } else if( result != SSH_AGAIN ) {

            timer_.stop();
            setErrorString( ssh_get_error(session) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;

        } else return false;

        #else
        return true;
        #endif

    }

    //_______________________________________________________________________
    bool Socket::_tryRead()
    {

        if( !isConnected() ) return false;

        #if WITH_SSH

        // cash channel
        auto channel = static_cast<ssh_channel>(channel_.get());

        // pol the channel, assuming this is faster than reading
        auto available = ssh_channel_poll( channel, false );
        if( available == 0 ) return false;
        else if( available == SSH_EOF )
        {

            timer_.stop();
            setErrorString( "channel closed" );
            emit readChannelFinished();
            return true;

        } else if( available == SSH_ERROR ) {


            setErrorString( tr( "invalid read" ) );
            timer_.stop();
            bytesAvailable_ = -1;
            return false;

        }

        // read from channel
        auto length =  ssh_channel_read( channel, buffer_.data()+bytesAvailable_, maxBufferSize-bytesAvailable_, false );
        if( length == SSH_AGAIN ) return false ;
        else if( length < 0 )
        {

            setErrorString( tr( "invalid read" ) );
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

    //______________________________________________________
    void Socket::ChannelDeleter::operator() (void* ptr) const
    {
        #if WITH_SSH
        auto channel( static_cast<ssh_channel>(ptr) );
        if( ssh_channel_is_open( channel ) ) ssh_channel_close( channel );
        ssh_channel_free( channel );
        #endif
    }
}

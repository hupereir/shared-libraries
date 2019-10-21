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

#include "SshReadFileSocket.h"
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
    ReadFileSocket::ReadFileSocket( QObject* parent ):
        QIODevice( parent ),
        Counter( "Ssh::ReadFileSocket" )
    {
        buffer_.resize( maxBufferSize );
        setOpenMode(QIODevice::ReadOnly);
    }

    //_______________________________________________________________________
    ReadFileSocket::~ReadFileSocket()
    { close(); }

    //_______________________________________________________________________
    bool ReadFileSocket::atEnd() const
    { return !isConnected() || bytesRead_ == fileSize_; }

    //_______________________________________________________________________
    void ReadFileSocket::connect( void* session, const QString& file )
    {

        /*
        TODO
        TryConnect always return true (either succeeds or fails but never require rerun
        One can therefore call it directly, remove it from the timer and remove the "waitForConnected" method
        */

        // store session, host and port
        session_ = session;
        remoteFile_ = file;

        // run timer and try connect
        if( timer_.isActive() ) timer_.stop();
        if( !_tryConnect() ) timer_.start( latency_, this );
    }

    //_______________________________________________________________________
    bool ReadFileSocket::waitForConnected( int msecs )
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
    void ReadFileSocket::close()
    {
        Debug::Throw( "Ssh::ReadFileSocket:close.\n" );

        // stop timer
        if( timer_.isActive() ) timer_.stop();
        channel_.reset();

    }

    //_______________________________________________________________________
    qint64 ReadFileSocket::readData( char* data, qint64 maxSize )
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
    void ReadFileSocket::timerEvent( QTimerEvent* event )
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
    bool ReadFileSocket::_tryConnect()
    {

        if( isConnected() ) return true;

        #if WITH_SSH

        // cast session
        auto session( static_cast<ssh_session>(session_) );

        // create and initialize channel if not already done
        auto channel( static_cast<ssh_scp>(channel_.get()) );
        if( !channel )
        {

            // create channel
            channel = ssh_scp_new(session, SSH_SCP_READ, qPrintable( remoteFile_ ) );
            if( !channel )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            // initialize
            if( ssh_scp_init( channel ) != SSH_OK )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            // assign
            channel_.reset( channel );
        }

        // open connection to file
        if( ssh_scp_pull_request( channel ) != SSH_OK )
        {
            timer_.stop();
            setErrorString( ssh_get_error(session) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        // get file size
        fileSize_ = ssh_scp_request_get_size64( channel );

        // accept transfer
        ssh_scp_accept_request( channel );

        // mark as connected
        connected_ = true;
        emit connected();
        return true;

        #else
        return true;
        #endif

    }

    //_______________________________________________________________________
    bool ReadFileSocket::_tryRead()
    {

        if( !isConnected() ) return false;

        #if WITH_SSH

        // cash channel
        auto session( static_cast<ssh_session>(session_) );
        auto channel = static_cast<ssh_scp>(channel_.get());

        // read from channel
        auto length =  ssh_scp_read( channel, buffer_.data()+bytesAvailable_, maxBufferSize-bytesAvailable_ );
        if( length == 0 ) return false ;
        else if( length == SSH_ERROR )
        {

            setErrorString( tr( "invalid read - %1" ).arg( ssh_get_error( session ) ) );
            timer_.stop();
            bytesAvailable_ = -1;
            return false;

        } else {

            bytesAvailable_ += length;
            bytesRead_ += length;
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
    void ReadFileSocket::ChannelDeleter::operator() (void* ptr) const
    {
        #if WITH_SSH
        auto channel( static_cast<ssh_scp>(ptr) );
        ssh_scp_close( channel );
        ssh_scp_free( channel );
        #endif
    }
}

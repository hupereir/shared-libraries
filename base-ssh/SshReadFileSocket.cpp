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
#include <libssh/sftp.h>
#endif

#include <fcntl.h>

namespace Ssh
{

    //* max buffer size
    static const qint64 bufferSize = 1024*64;
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
        Debug::Throw() << "Ssh::ReadFileSocket::connect - file: " << file << endl;

        // store session, host and port
        session_ = session;
        remoteFileName_ = file;

        // run timer and try connect
        if( !timer_.isActive() )
        { timer_.start( latency_, this ); }

    }

    //_______________________________________________________________________
    bool ReadFileSocket::waitForConnected( int msecs )
    {

        Debug::Throw( "Ssh::ReadFileSocket::waitForConnected.\n" );

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
        handle_.reset();
        sftp_.reset();
        fileSize_ = 0;
        remoteFileName_.clear();

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

        Debug::Throw( "Ssh::ReadFileSocket::_tryConnect.\n" );
        if( isConnected() ) return true;

        #if WITH_SSH

        // cast session
        auto session( static_cast<ssh_session>(session_) );
        Util::SessionBlocker blocker( session );

        // invalid termination sequence
        auto terminate = [this, session]()
        {
            timer_.stop();
            setErrorString( ssh_get_error(session) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        };

        // create sftp session
        auto sftp( static_cast<sftp_session>(sftp_.get()) );
        if( !sftp )
        {
            // create sftp session and initialize
            sftp = sftp_new( session );
            if( !( sftp && sftp_init( sftp ) == SSH_OK  ) )
            { return terminate(); }

            // store
            sftp_.reset( sftp );
        }

        // file handle
        auto handle = static_cast<sftp_file>( handle_.get() );
        if( !handle )
        {
            // open
            handle = sftp_open( sftp, qPrintable( remoteFileName_ ), O_RDONLY, 0 );
            if( !handle ) return terminate();

            // store
            handle_.reset( handle );
            fileSize_ = 0;

        }

        // file size
        if( !fileSize_ )
        {
            auto attributes = sftp_stat( sftp, qPrintable( remoteFileName_ ) );
            if( !attributes ) return terminate();

            fileSize_ = attributes->size;
            sftp_attributes_free( attributes );
            Debug::Throw() << "Ssh::ReadFileSocket::_tryConnect - file size: " << fileSize_ << endl;

        }

        // create ids
        for( int i = 0; i < nParallel; ++i )
        {
            if( fileSize_ < i*bufferSize ) break;
            if( ( idList_[i] = sftp_async_read_begin( handle, bufferSize ) ) == SSH_ERROR )
            { return terminate(); }
        }

        currentIndex_ = 0;

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

        Debug::Throw( "Ssh::ReadFileSocket::_tryRead.\n" );

        if( !isConnected() ) return false;
        if( maxBufferSize - bytesAvailable_ < bufferSize ) return true;

        #if WITH_SSH

        // cash channel
        auto session( static_cast<ssh_session>(session_) );
        auto handle = static_cast<sftp_file>(handle_.get());

        // invalid termination sequence
        auto terminate = [this]( const QString& message)
        {
            setErrorString( message );
            emit error( QAbstractSocket::UnknownSocketError );
            timer_.stop();
            bytesAvailable_ = -1;
            return false;
        };

        auto read =  sftp_async_read( handle, buffer_.data()+bytesAvailable_, bufferSize, idList_[currentIndex_] );
        if( read == SSH_ERROR ) return terminate( tr( "invalid read - %1" ).arg( ssh_get_error( session ) ) );

        bytesRead_ += read;
        bytesAvailable_ += read;
        if( read != bufferSize && bytesRead_ < fileSize_ ) return terminate( tr( "invalid read - file too short" ) );

        emit readyRead();

        // check if need relaunch
        auto remaining = fileSize_ - bytesRead_;
        if( remaining < bufferSize*(nParallel-1) ) idList_[currentIndex_] = -1;
        else {
            // truncate
            if( remaining > bufferSize ) remaining = bufferSize;
            if( ( idList_[currentIndex_] = sftp_async_read_begin( handle, remaining ) ) == SSH_ERROR )
            { return terminate( tr( "invalid read - %1" ).arg( ssh_get_error(session) ) ); }
        }

        // update index
        currentIndex_ = (currentIndex_+1)%nParallel;

        return true;

        #else
        return false;
        #endif

    }

}

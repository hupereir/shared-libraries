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

        class SessionBlocker
        {
            public:

            SessionBlocker( ssh_session session ):
                session_( session )
            { ssh_set_blocking(session_, true); }

            ~SessionBlocker()
            { ssh_set_blocking(session_, false); }

            private:

            ssh_session session_;

        };

        // cast session
        auto session( static_cast<ssh_session>(session_) );
        SessionBlocker blocker( session );

        // create sftp
        auto sftp( static_cast<sftp_session>(sftp_.get()) );
        if( !sftp )
        {

            // create session
            sftp = sftp_new( session );
            if( !sftp )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            Debug::Throw( "Ssh::ReadFileSocket::_tryConnect - sftp new done.\n" );

            // initialize
            if( sftp_init( sftp ) != SSH_OK  )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            sftp_.reset( sftp );
            Debug::Throw( "Ssh::ReadFileSocket::_tryConnect - sftp created.\n" );

        }

        // file handle
        auto handle = static_cast<sftp_file>( handle_.get() );
        if( !handle )
        {
            // open
            handle = sftp_open( sftp, qPrintable( remoteFileName_ ), O_RDONLY, 0 );
            if( !handle )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            // store
            handle_.reset( handle );
            fileSize_ = 0;

            Debug::Throw( "Ssh::ReadFileSocket::_tryConnect - handle created.\n" );

        }

        // file size
        if( !fileSize_ )
        {
            auto attributes = sftp_stat( sftp, qPrintable( remoteFileName_ ) );
            if( !attributes )
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }

            fileSize_ = attributes->size;
            sftp_attributes_free( attributes );
            Debug::Throw() << "Ssh::ReadFileSocket::_tryConnect - file size: " << fileSize_ << endl;

        }

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

        #if WITH_SSH

        // cash channel
        auto session( static_cast<ssh_session>(session_) );
        auto handle = static_cast<sftp_file>(handle_.get());

        // read from channel
        auto read =  sftp_read( handle, buffer_.data()+bytesAvailable_, maxBufferSize-bytesAvailable_ );
        if( read == SSH_ERROR )
        {

            setErrorString( tr( "invalid read - %1" ).arg( ssh_get_error( session ) ) );
            timer_.stop();
            bytesAvailable_ = -1;
            return false;

        } else if( read == SSH_AGAIN ) {

            return false;

        } else if( read == 0 ) {

            timer_.stop();
            setErrorString( "completed" );
            emit readChannelFinished();

        } else {

            bytesAvailable_ += read;
            bytesRead_ += read;
            emit readyRead();

        }
        return true;

        #else
        return false;
        #endif

    }

    //______________________________________________________
    void ReadFileSocket::SftpDeleter::operator() (void* ptr) const
    {
        #if WITH_SSH
        auto sftp = static_cast<sftp_session>( ptr );
        sftp_free( sftp );
        #endif
    }

    //______________________________________________________
    void ReadFileSocket::FileHandleDeleter::operator() (void* ptr) const
    {
        #if WITH_SSH
        auto file = static_cast<sftp_file>( ptr );
        sftp_close( file );
        #endif
    }

}

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

#include "SshWriteFileSocket.h"
#include "Debug.h"

#include <QElapsedTimer>

#if WITH_SSH
#include <libssh/libssh.h>
#endif

#if defined(Q_OS_WIN)
/* File mode */
/* Read, write, execute/search by owner */
#define S_IRWXU 0000700 /* RWX mask for owner */
#define S_IRUSR 0000400 /* R for owner */
#define S_IWUSR 0000200 /* W for owner */
#define S_IXUSR 0000100 /* X for owner */
/* Read, write, execute/search by group */
#define S_IRWXG 0000070 /* RWX mask for group */
#define S_IRGRP 0000040 /* R for group */
#define S_IWGRP 0000020 /* W for group */
#define S_IXGRP 0000010 /* X for group */
/* Read, write, execute/search by others */
#define S_IRWXO 0000007 /* RWX mask for other */
#define S_IROTH 0000004 /* R for other */
#define S_IWOTH 0000002 /* W for other */
#define S_IXOTH 0000001 /* X for other */
#else
#include <sys/stat.h>
#endif

namespace Ssh
{

    //_______________________________________________________________________
    WriteFileSocket::WriteFileSocket( QObject* parent ):
        QIODevice( parent ),
        Counter( "Ssh::WriteFileSocket" )
    { setOpenMode(QIODevice::WriteOnly); }

    //_______________________________________________________________________
    WriteFileSocket::~WriteFileSocket()
    { close(); }

    //_______________________________________________________________________
    void WriteFileSocket::connect( void* session, const QString& file, qint64 size )
    {

        /*
        TODO
        TryConnect always return true (either succeeds or fails but never require rerun
        One can therefore call it directly, remove it from the timer and remove the "waitForConnected" method
        */

        // store session, host and port
        session_ = session;
        remoteFileName_ = file;
        fileSize_ = size;

        // run timer and try connect
        if( !timer_.isActive() ) timer_.start( latency_, this );
    }

    //_______________________________________________________________________
    bool WriteFileSocket::waitForConnected( int msecs )
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
    void WriteFileSocket::close()
    {
        Debug::Throw( "Ssh::WriteFileSocket:close.\n" );

        // stop timer
        if( timer_.isActive() ) timer_.stop();
        channel_.reset();

    }

    //_______________________________________________________________________
    void WriteFileSocket::timerEvent( QTimerEvent* event )
    {
        // check timer id
        if( event->timerId() != timer_.timerId() ) return QIODevice::timerEvent( event );

        #if WITH_SSH

        if( !isConnected() ) _tryConnect();
        else timer_.stop();

        return;

        #else

        timer_.stop();
        bytesAvailable_ = -1;
        setErrorString( "no ssh" );
        return;

        #endif
    }

    //_______________________________________________________________________
    qint64 WriteFileSocket::writeData( const char* data, qint64 maxSize )
    {

        if( !isConnected() ) return -1;
        #if WITH_SSH

        auto session( static_cast<ssh_session>(session_) );
        auto channel = static_cast<ssh_scp>(channel_.get());
        auto result = ssh_scp_write( channel, data, maxSize );
        if( result == SSH_OK ) return maxSize;
        else
        {
            setErrorString( tr( "invalid write - %1" ).arg( ssh_get_error(session) ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return -1;
        }

        #else

        setErrorString( "invalid channel" );
        return -1;

        #endif

    }

    //_______________________________________________________________________
    bool WriteFileSocket::_tryConnect()
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
            channel = ssh_scp_new( session, SSH_SCP_WRITE, qPrintable( remoteFileName_ ) );
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
        if( ssh_scp_push_file64( channel, qPrintable( remoteFileName_ ), fileSize_, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH ) != SSH_OK )
        {
            timer_.stop();
            setErrorString( ssh_get_error(session) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        // mark as connected
        connected_ = true;
        emit connected();
        return true;

        #else
        return true;
        #endif

    }

    //______________________________________________________
    void WriteFileSocket::ChannelDeleter::operator() (void* ptr) const
    {
        #if WITH_SSH
        auto channel( static_cast<ssh_scp>(ptr) );
        ssh_scp_close( channel );
        ssh_scp_free( channel );
        #endif
    }
}

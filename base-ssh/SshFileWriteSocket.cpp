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

#include "SshFileWriteSocket.h"
#include "Debug.h"

#include <QElapsedTimer>

#if HAVE_SSH
#include <libssh2.h>
#endif

namespace Ssh
{

    //_______________________________________________________________________
    FileWriteSocket::FileWriteSocket( QObject* parent ):
        BaseSocket( parent )
        {}

    //_______________________________________________________________________
    void FileWriteSocket::connectToFile( void* session, const QString& path, quint64 size, int mode )
    {

        Debug::Throw() << "Ssh::FileWriteSocket::connectToFile - " << path << " size: " << size << endl;

        // store session, host and port
        session_ = session;
        path_ = path;
        size_ = size;
        mode_ = mode;

        // make sure timer is running
        if( timer_.isActive() ) timer_.stop();
        if( !_tryConnect() ) timer_.start( latency_, this );

    }

    //_______________________________________________________________________
    bool FileWriteSocket::waitForConnected( int msecs )
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
    void FileWriteSocket::timerEvent( QTimerEvent* event )
    {

        // check timer id
        if( event->timerId() == timer_.timerId() )
        {

            #if HAVE_SSH

            if( _tryConnect() ) timer_.stop();
            return;

            #else

            timer_.stop();
            setErrorString( "no ssh" );
            return;

            #endif

        } else return BaseSocket::timerEvent( event );

    }

    //_______________________________________________________________________
    bool FileWriteSocket::_tryConnect( void )
    {

        if( isConnected() ) return true;

        Debug::Throw( "FileWriteSocket::_tryConnect.\n" );

        #if HAVE_SSH
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );

        // 511 (dec) is 777 (oct)
        auto channel = libssh2_scp_send64( session, qPrintable( path_ ), mode_&0777, (libssh2_uint64_t) size_, 0, 0 );
        if( channel )
        {

            _setChannel( channel, QIODevice::WriteOnly );
            return true;

        } else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN ) {

            timer_.stop();
            char *errMsg(nullptr);
            libssh2_session_last_error(session, &errMsg, NULL, 0);
            setErrorString( tr( "error connecting to remote file %1: %2" ).arg( path_ ).arg( errMsg ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;

        }

        return false;

        #else
        return true;
        #endif

    }

}

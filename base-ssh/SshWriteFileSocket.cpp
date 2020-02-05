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

#include "BaseFileInfo.h"
#include "Debug.h"

#include <QElapsedTimer>

#if WITH_SSH
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#endif

#include <fcntl.h>

static const QFile::Permissions defaultPermissions( QFile::ReadOwner|QFile::WriteOwner|QFile::ReadGroup|QFile::ReadOther );


namespace Ssh
{

    //_______________________________________________________________________
    WriteFileSocket::WriteFileSocket( QObject* parent ):
        QIODevice( parent ),
        Counter( QStringLiteral("Ssh::WriteFileSocket") )
    { setOpenMode(QIODevice::WriteOnly); }

    //_______________________________________________________________________
    WriteFileSocket::~WriteFileSocket()
    { close(); }

    //_______________________________________________________________________
    void WriteFileSocket::connect( void* session, const QString& file, qint64 size, QFile::Permissions permissions )
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
        permissions_ = permissions;

        // run timer and try connect
        if( !timer_.isActive() )
        { timer_.start( latency_, this ); }

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
        Debug::Throw( QStringLiteral("Ssh::WriteFileSocket:close.\n") );

        // stop timer
        if( timer_.isActive() ) timer_.stop();
        handle_.reset();
        sftp_.reset();
        fileSize_ = 0;
        remoteFileName_.clear();

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
        auto handle = static_cast<sftp_file>(handle_.get());
        auto result = sftp_write( handle, data, maxSize );
        if( result == SSH_ERROR )
        {

            setErrorString( tr( "invalid write - %1" ).arg( ssh_get_error(session) ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return -1;

        } else return result;

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

        // create and initialize channel if not already done
        auto handle( static_cast<sftp_file>(handle_.get()) );
        if( !handle )
        {
            // open destination
            auto handle = sftp_open( sftp, qPrintable( remoteFileName_ ),
                O_WRONLY|O_CREAT|O_TRUNC,
                BaseFileInfo::unixPermissions( permissions_ ? permissions_:defaultPermissions ) );
            if( !handle ) return terminate();

            // store
            handle_.reset( handle );
        }

        // mark as connected
        connected_ = true;
        emit connected();
        return true;

        #else
        return true;
        #endif

    }

}

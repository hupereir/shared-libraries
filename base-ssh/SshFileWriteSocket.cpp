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
        _addCommand( Connect );

    }

    //_______________________________________________________________________
    void FileWriteSocket::sendEof( void )
    {
        _addCommand( SendEof );
        _addCommand( WaitForEof );
        _addCommand( WaitForClosed );
    }

    //_______________________________________________________________________
    bool FileWriteSocket::waitForConnected( int msecs )
    {

        // do nothing if already connected
        if( isConnected() ) return true;

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        { if( !_processCommands() ) return true; }

        return false;

    }

    //_______________________________________________________________________
    bool FileWriteSocket::waitForCompleted( int msecs )
    {

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        { if( !_processCommands() ) return true; }

        return false;

    }

    //_______________________________________________________________________
    void FileWriteSocket::timerEvent( QTimerEvent* event )
    {

        // check timer id
        if( event->timerId() == timer_.timerId() )
        {

            if( !_processCommands() && timer_.isActive() ) timer_.stop();

        } else return BaseSocket::timerEvent( event );

    }

    //_______________________________________________
    void FileWriteSocket::_addCommand( Command command )
    {
        Debug::Throw() << "Ssh::FileWriteSocket::_AddCommand: " << command << endl;
        commands_.append( command );
        if( !timer_.isActive() ) timer_.start( _latency(), this );
    }


    //_______________________________________________
    bool FileWriteSocket::_processCommands( void )
    {

        // check empty commands list
        if( commands_.empty() ) return false;

        switch( commands_.front() )
        {

            case Connect:
            {
                if( _tryConnect() && !commands_.empty() ) commands_.removeFirst();
                return true;
            }

            case SendEof:
            {
                if( _trySendEof() && !commands_.empty() ) commands_.removeFirst();
                return true;
            }


            case WaitForEof:
            {
                if( _tryWaitEof() && !commands_.empty() ) commands_.removeFirst();
                return true;
            }

            case WaitForClosed:
            {
                if( _tryClose() && !commands_.empty() ) commands_.removeFirst();
                return true;
            }

            default:
            if( !commands_.empty() )
            { commands_.removeFirst(); }
            return true;

        }

        return false;

    }

    //_______________________________________________________________________
    bool FileWriteSocket::_tryConnect( void )
    {

        if( isConnected() ) return true;

        Debug::Throw( "FileWriteSocket::_tryConnect.\n" );

        #if HAVE_SSH
        const auto session( static_cast<LIBSSH2_SESSION*>(session_) );
        if( !session )
        {
            setErrorString( tr( "invalid session" ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        const auto channel = libssh2_scp_send64( session, qPrintable( path_ ), mode_&0777, (libssh2_uint64_t) size_, 0, 0 );
        if( channel )
        {

            _setChannel( channel, QIODevice::WriteOnly );
            return true;

        } else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN ) {

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

    //_______________________________________________________________________
    bool FileWriteSocket::_trySendEof( void )
    {

        Debug::Throw( "FileWriteSocket::_trySendEof.\n" );

        #if HAVE_SSH
        const auto session( static_cast<LIBSSH2_SESSION*>(session_) );
        if( !session )
        {
            setErrorString( tr( "invalid session" ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        const auto channel( static_cast<LIBSSH2_CHANNEL*>(_channel() ) );
        int result = libssh2_channel_send_eof( channel );

        if( result == 0 ) return true;
        else if( result == LIBSSH2_ERROR_EAGAIN ) return false;
        else {

            char *errMsg(nullptr);
            libssh2_session_last_error(session, &errMsg, NULL, 0);
            setErrorString( tr( "error sending eof to remote file %1: %2" ).arg( path_ ).arg( errMsg ) );
            emit error( QAbstractSocket::UnknownSocketError );
            return true;

        }

        #else
        setErrorString( tr( "no ssh" ) );
        return true;
        #endif

    }

    //_______________________________________________________________________
    bool FileWriteSocket::_tryWaitEof( void )
    {

        Debug::Throw( "FileWriteSocket::_tryWaitEof.\n" );

        #if HAVE_SSH
        const auto session( static_cast<LIBSSH2_SESSION*>(session_) );
        if( !session )
        {
            setErrorString( tr( "invalid session" ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        const auto channel( static_cast<LIBSSH2_CHANNEL*>(_channel() ) );
        int result = libssh2_channel_wait_eof( channel );
        if( result == 0 ) return true;
        else if( result == LIBSSH2_ERROR_EAGAIN ) return false;
        else {

            char *errMsg(nullptr);
            libssh2_session_last_error(session, &errMsg, NULL, 0);
            setErrorString( tr( "error waiting for eof from remote file %1: %2" ).arg( path_ ).arg( errMsg ) );
            emit error( QAbstractSocket::UnknownSocketError );
            return true;

        }

        #else
        setErrorString( tr( "no ssh" ) );
        return true;
        #endif

    }

    //_______________________________________________________________________
    bool FileWriteSocket::_tryClose( void )
    {

        Debug::Throw( "FileWriteSocket::_tryClose.\n" );

        #if HAVE_SSH
        const auto session( static_cast<LIBSSH2_SESSION*>(session_) );
        if( !session )
        {
            setErrorString( tr( "invalid session" ) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;
        }

        const auto channel( static_cast<LIBSSH2_CHANNEL*>(_channel() ) );
        int result = libssh2_channel_wait_closed( channel );
        if( result == 0 ) return true;
        else if( result == LIBSSH2_ERROR_EAGAIN ) return false;
        else {

            char *errMsg(nullptr);
            libssh2_session_last_error(session, &errMsg, NULL, 0);
            setErrorString( tr( "error waiting for closed from remote file %1: %2" ).arg( path_ ).arg( errMsg ) );
            emit error( QAbstractSocket::UnknownSocketError );
            return true;

        }

        #else
        setErrorString( tr( "no ssh" ) );
        return true;
        #endif

    }

}

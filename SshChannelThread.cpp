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

#include "SshChannelThread.h"

#include "Debug.h"

#include <QMutexLocker>

#if defined (Q_OS_WIN)
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <unistd.h>

#if HAVE_SSH
#include <libssh2.h>
#endif

namespace Ssh
{

    //_______________________________________________________________
    ChannelThread::ChannelThread( QObject* parent, const TunnelAttributes& attributes, void* session, int socket ):
        QThread( parent ),
        Counter( "Ssh::ChannelThread" ),
        attributes_( attributes ),
        session_( session ),
        socket_( socket )
    { Debug::Throw( "Ssh::ChannelThread::ChannelThread.\n" ); }

    //_______________________________________________________________
    ChannelThread::~ChannelThread( void )
    {
        Debug::Throw( "SSh::ChannelThread:~ChannelThread.\n" );
        close();
    }

    //_______________________________________________________________
    void ChannelThread::close( void )
    {


        // close ssh channel
        if( channel_ )
        {
            #if HAVE_SSH
            QMutexLocker lock( mutex_ );
            libssh2_channel_free( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
            channel_ = nullptr;
            #endif
        }

        // close socket
        if( socket_ >= 0 )
        {
            Debug::Throw( "SSh::ChannelThread::close.\n" );
            ::close( socket_ );
            socket_ = -1;
        }

    }

    //_______________________________________________________________
    void ChannelThread::initialize( void )
    {

        if( channel_ ) return;
        Debug::Throw() << "Ssh::ChannelThread::initialize - host: " << attributes_.host() << ":" << attributes_.remotePort() << endl;

        #if HAVE_SSH
        LIBSSH2_SESSION* session( reinterpret_cast<LIBSSH2_SESSION*>(session_) );
        forever
        {
            channel_ = libssh2_channel_direct_tcpip(
                session,
                qPrintable( attributes_.host() ),
                attributes_.remotePort() );

            if( channel_ ) break;
            else if( libssh2_session_last_errno( session ) != LIBSSH2_ERROR_EAGAIN )
            {
                char *err_msg;
                libssh2_session_last_error(session, &err_msg, NULL, 0);
                Debug::Throw() << "ChannelThread::run - error getting direct tpc channel: " << err_msg << endl;
                emit error( tr( "Failed to create channel to %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                return;
            }

        }
        #endif

    }

    //_______________________________________________________________
    void ChannelThread::run( void )
    {

        // check channel
        if( !channel_ )
        {
            emit error( tr( "Invalid channel" ) );
            return;
        }

        #if HAVE_SSH
        // cast channel
        LIBSSH2_CHANNEL* channel = reinterpret_cast<LIBSSH2_CHANNEL*>(channel_);

        // starting from here, this should go to a separate thread
        QByteArray buffer;
        buffer.resize(16384);
        forever
        {

            fd_set fileDescriptor;
            FD_ZERO(&fileDescriptor);
            FD_SET(socket_, &fileDescriptor);

            struct timeval timeValue;
            timeValue.tv_sec = 0;
            timeValue.tv_usec = 100000;

            // select method is blocking
            const int result = select( socket_ + 1, &fileDescriptor, nullptr, nullptr, &timeValue );
            if( result == -1 )
            {
                emit error( tr( "Error selecting socket forwarded to %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                return;
            }

            // read from socket, write through ssh
            if( result && FD_ISSET(socket_, &fileDescriptor ) )
            {
                ssize_t bytesRead = recv(socket_, buffer.data(), buffer.size(), 0);
                if( bytesRead<0 )
                {
                    emit error( tr( "Error reading from port %1" ).arg( attributes_.localPort() ) );
                    return;

                } else if( bytesRead == 0 ) {

                    emit debug( QString( "Ssh::ChannelThread::run - local socket on port %1 disconnected" ).arg( attributes_.localPort() ) );
                    return;

                }

                emit debug( QString( "Ssh::ChannelThread::run - fromTcpSocket - bytesRead=%1" ).arg( bytesRead ) );

                QMutexLocker lock( mutex_ );
                ssize_t bytesWritten = 0;
                ssize_t i = 0;
                do
                {
                    i = libssh2_channel_write( channel, buffer.data()+bytesWritten, bytesRead-bytesWritten );

                    if (i < 0)
                    {
                        emit error( tr( "Error writting to %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                        return;
                    }
                    bytesWritten += i;

                } while( i > 0 && bytesWritten < bytesRead );
            }

            // read from ssh, write to socket
            forever
            {

                QMutexLocker lock( mutex_ );
                ssize_t bytesRead = libssh2_channel_read( channel, buffer.data(), buffer.size() );
                if( bytesRead == LIBSSH2_ERROR_EAGAIN ) break;
                else if( bytesRead < 0 )
                {
                    emit error( tr( "Error reading from %1:%2 (%3)" ).arg( attributes_.host() ).arg( attributes_.remotePort() ).arg( bytesRead ) );
                    return;
                }

                emit debug( QString( "Ssh::ChannelThread::run - fromSshSocket - bytesRead=%1" ).arg( bytesRead ) );

                ssize_t bytesWritten = 0;
                ssize_t i = 0;
                while( bytesWritten < bytesRead )
                {
                    i = send(socket_, buffer.data() + bytesWritten, bytesRead - bytesWritten, 0);
                    if (i <= 0)
                    {

                        emit error( tr( "Error writing to port %1" ).arg( attributes_.localPort() ) );
                        return;
                    }
                    bytesWritten += i;
                }

                if (libssh2_channel_eof( channel ) )
                {
                    emit debug( "Ssh::ChannelThread::run - end of channel" );
                    return;
                }
            }
        }
        #endif

        close();

    }

}

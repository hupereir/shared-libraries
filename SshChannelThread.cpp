
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

#if HAVE_SSH
#include <libssh2.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
// #include <arpa/inet.h>

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
    void ChannelThread::close( void )
    {
        // close socket
        if( socket_ >= 0 ) ::close( socket_ );
        socket_ = -1;

        // close ssh channel
        if( channel_ )
        {
            #if HAVE_SSH
            libssh2_channel_free( reinterpret_cast<LIBSSH2_CHANNEL*>(channel_) );
            #endif
        }
        channel_ = nullptr;

    }

    //_______________________________________________________________
    void ChannelThread::run( void )
    {
        Debug::Throw( "Ssh::ChannelThread::run.\n" );

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

            const int result = select( socket_ + 1, &fileDescriptor, nullptr, nullptr, &timeValue );
            if( result == -1 )
            {
                emit error( tr( "Error selecting socket forwarded to %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                perror("select");
                return;
            }

            // read from socket, write through ssh
            if( result && FD_ISSET(socket_, &fileDescriptor ) )
            {
                ssize_t length = recv(socket_, buffer.data(), buffer.size(), 0);
                if( length<0 )
                {
                    emit error( tr( "Error reading from port %1" ).arg( attributes_.localPort() ) );
                    perror("read");
                    return;

                } else if( length == 0 ) {

                    emit error( tr( "Local socket on port %1 disconnected" ).arg( attributes_.localPort() ) );
                    return;

                }

                ssize_t bytesWritten = 0;
                ssize_t i = 0;
                do
                {
                    i = libssh2_channel_write( channel, buffer.data(), length );

                    if (i < 0)
                    {
                        emit error( tr( "Error writting to %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                        return;
                    }
                    bytesWritten += i;

                } while( i > 0 && bytesWritten < length );
            }

            // read from ssh, write to socket
            forever
            {

                ssize_t length = libssh2_channel_read( channel, buffer.data(), buffer.size() );
                if( length == LIBSSH2_ERROR_EAGAIN ) break;
                else if( length < 0 )
                {
                    emit error( tr( "Error reading from %1:%2" ).arg( attributes_.host() ).arg( attributes_.remotePort() ) );
                    return;
                }

                ssize_t bytesWritten = 0;
                ssize_t i = 0;
                while( bytesWritten < length )
                {
                    i = send(socket_, buffer.data() + bytesWritten, length - bytesWritten, 0);
                    if (i <= 0)
                    {

                        emit error( tr( "Error writing to port %1" ).arg( attributes_.localPort() ) );
                        perror("write");
                        return;
                    }
                    bytesWritten += i;
                }

                if (libssh2_channel_eof( channel ) )
                {
                    emit error( tr( "End of channel" ) );
                    return;
                }
            }
        }
        #endif

        Debug::Throw() << "Ssh::ChannelThread::run - finished." << endl;
        close();

    }

}

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

#include "SshUtil.h"

#include <QList>

#if defined(Q_OS_WIN)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#include <unistd.h>
#include <fcntl.h>

namespace Ssh
{

    //____________________________________________________
    uint32_t Util::tcpOptions( int socket )
    {
        int optionNames[] =
        {
            #if  !defined(Q_OS_WIN)
            TCP_CORK, TCP_DEFER_ACCEPT, TCP_INFO, TCP_KEEPCNT, TCP_KEEPIDLE,
            TCP_KEEPINTVL, TCP_LINGER2, TCP_MAXSEG, TCP_QUICKACK,  TCP_SYNCNT,
            TCP_WINDOW_CLAMP,
            #endif
            TCP_NODELAY, 0
        };

        uint32_t result = 0;
        int bit = 0;
        for( int index = 0; optionNames[index]; ++index )
        {
            #if defined( Q_OS_WIN )
            char value = 0;
            #else
            int value = 0;
            #endif
            socklen_t length = 0;
            if( getsockopt( socket, IPPROTO_TCP, optionNames[index], &value, &length ) == 0 && value )
            { result |= (1<<bit); }

            if( value ) result |= (1<<bit);
            ++bit;
        }

        return result;

    }

    //____________________________________________________
    uint32_t Util::socketOptions( int socket )
    {
        int optionNames[] =
        {
            SO_ACCEPTCONN, SO_BROADCAST, SO_DEBUG,SO_ERROR, SO_DONTROUTE,
            SO_KEEPALIVE, SO_LINGER,
            #if !defined( Q_OS_WIN )
            SO_BINDTODEVICE,  SO_BSDCOMPAT, SO_DOMAIN, SO_MARK, SO_PASSCRED,
            SO_PEERCRED,  SO_PRIORITY,  SO_PROTOCOL, SO_RCVBUFFORCE, SO_SNDBUFFORCE,
            SO_TIMESTAMP,
            #endif
            SO_OOBINLINE, SO_RCVBUF, SO_RCVLOWAT, SO_SNDLOWAT, SO_RCVTIMEO,
            SO_SNDTIMEO, SO_REUSEADDR, SO_SNDBUF, SO_TYPE,
            0
        };

        uint32_t result = 0;
        int bit = 0;
        for( int index = 0; optionNames[index]; ++index )
        {
            #if defined( Q_OS_WIN )
            char value = 0;
            #else
            int value = 0;
            #endif

            socklen_t length = 0;
            if( getsockopt( socket, SOL_SOCKET, optionNames[index], &value, &length ) == 0 && value )
            { result |= (1<<bit); }
            ++bit;
        }

        return result;

    }

    //____________________________________________________
    uint32_t Util::fileDescriptorOptions( int fd )
    {
        #if defined(Q_OS_WIN)
        return 0;
        #else
        return fcntl( fd, F_GETFL );
        #endif
    }

}
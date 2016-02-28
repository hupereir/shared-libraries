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

#include "SshListeningThread.h"
#include "Debug.h"

#include <QHostAddress>

#if defined (Q_OS_WIN)
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <unistd.h>

#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)-1
#endif

namespace Ssh
{

    //_____________________________________________________________
    ListeningThread::ListeningThread( QObject* parent, const TunnelAttributes& attributes ):
        QThread( parent ),
        Counter( "Ssh::ListeningThread" ),
        attributes_( attributes )
    {
        Debug::Throw()
            << "Ssh::ListeningThread::ListeningThread -"
            << " host: " << attributes_.host()
            << " local port: " << attributes_.localPort()
            << " remote port: " << attributes_.remotePort()
            << endl;
    }

    //_____________________________________________________________
    ListeningThread::~ListeningThread(void )
    {
        Debug::Throw( "Ssh::ListeningThread::~ListeningThread.\n" );
        close();
    }

    //_____________________________________________________________
    void ListeningThread::close( void )
    {
        if( socket_ >= 0 )
        {
            Debug::Throw( "Ssh::ListeningThread::close." );
            ::close( socket_ );
        }

        socket_ = -1;
    }

    //_____________________________________________________________
    void ListeningThread::run( void )
    {

        // socket address
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons( attributes_.localPort() );
        address.sin_addr.s_addr = htonl( QHostAddress( QHostAddress::LocalHost ).toIPv4Address() );
        if( address.sin_addr.s_addr == INADDR_NONE )
        {
            _setErrorString( QString( "Invalid local host address" ) );
            return;
        }

        // socket
        socket_ = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if( socket_ < 0 )
        {
            _setErrorString( tr( "Invalid socket" ) );
            return;
        }

        #if defined(Q_OS_WIN)
        char socketOption = 1;
        #else
        int socketOption = 1;
        #endif

        setsockopt( socket_, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption) );

        // bind
        socklen_t addressLength = sizeof(address);
        if( bind(socket_, (struct sockaddr *)&address, addressLength) == -1 )
        {
            _setErrorString( tr( "Could not bind socket to port %1 on localhost" ).arg( attributes_.localPort() ) );
            return;
        }

        // listen
        if( listen( socket_, 2 ) == -1 )
        {
            _setErrorString( tr( "Could listen to port %1 on localhost" ).arg( attributes_.localPort() ) );
            return;
        }

        // infinite loop to accept connections
        forever
        {

            int forwardSocket = accept( socket_, (struct sockaddr *)&address, &addressLength);
            if( forwardSocket == -1 )
            {

                _setErrorString( tr( "Failed forwarding connection to port %1 on localhost" ).arg( attributes_.localPort() ) );
                return;

            } else {

                emit newConnection( attributes_.localPort(), forwardSocket );

            }

        }

    }

}

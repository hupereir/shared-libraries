
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

#include <sys/socket.h>
#include <unistd.h>

namespace Ssh
{

    //_____________________________________________________________
    ListeningThread::ListeningThread( QObject* parent, const TunnelAttributes& attributes ):
        QThread( parent ),
        Counter( "SSh::ListeningThread" ),
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
        Debug::Throw(0, "SSh::ListeningThread::~ListeningThread.\n" );
        close();
    }

    //_____________________________________________________________
    void ListeningThread::close( void )
    {
        if( socket_ >= 0 )
        {
            Debug::Throw(0, "SSh::ListeningThread::close.\n" );
            ::close( socket_ );
        }

        socket_ = -1;
    }

    //_____________________________________________________________
    void ListeningThread::initialize( void )
    {
        Debug::Throw(0) << "SSh::ListeningThread::initialize - port: " << attributes_.localPort() << endl;

        // socket address
        address_.sin_family = AF_INET;
        address_.sin_port = htons( attributes_.localPort() );
        address_.sin_addr.s_addr = htonl( QHostAddress( QHostAddress::LocalHost ).toIPv4Address() );
        if( address_.sin_addr.s_addr == INADDR_NONE )
        {
            emit error( QString( "Invalid local host address" ) );
            return;
        }

        // socket
        socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        int socketOption = 1;
        setsockopt( socket_, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption) );

        // bind
        socklen_t addressLength = sizeof(address_);
        if( bind(socket_, (struct sockaddr *)&address_, addressLength) == -1 )
        {
            emit error( tr( "Could not bind socket to port %1 on localhost" ).arg( attributes_.localPort() ) );
            perror( "bind" );
            return;
        }

        // listen
        if( listen( socket_, 2 ) == -1 )
        {
            emit error( tr( "Could listen to port %1 on localhost" ).arg( attributes_.localPort() ) );
            perror("listen");
            return;
        }

    }

    //_____________________________________________________________
    void ListeningThread::run( void )
    {
        Debug::Throw(0) << "SSh::ListeningThread::run - port: " << attributes_.localPort() << endl;

        if( socket_ < 0 )
        {
            emit error( tr( "Invalid socket" ) );
            return;
        }

        // infinite loop to accept connections
        socklen_t addressLength = sizeof(address_);
        forever
        {

            int forwardSocket = accept( socket_, (struct sockaddr *)&address_, &addressLength);
            if( forwardSocket == -1 )
            {

                emit error( tr( "Failed forwarding connection to port %1 on localhost" ).arg( attributes_.localPort() ) );
                perror("accept");
                return;

            } else {

                emit newConnection( attributes_.localPort(), forwardSocket );

            }

        }

        Debug::Throw() << "Ssh::ListeningThread::run - finished." << endl;
        close();

    }

}

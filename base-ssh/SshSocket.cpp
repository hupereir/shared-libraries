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

#include "SshSocket.h"
#include "Debug.h"

#include <QElapsedTimer>

#if WITH_SSH
#include <libssh/libssh.h>
#endif

namespace Ssh
{

    //_______________________________________________________________________
    Socket::Socket( QObject* parent ):
        BaseSocket( parent )
        {}

    //_______________________________________________________________________
    void Socket::connectToHost( void* session, const QString& host, quint16 port )
    {

        Debug::Throw() << "Ssh::Socket::connectToHost - " << host << ":" << port << endl;

        // store session, host and port
        session_ = session;
        host_ = host;
        port_ = port;

        // make sure timer is running
        if( timer_.isActive() ) timer_.stop();
        if( !_tryConnect() ) timer_.start( _latency(), this );

    }

    //_______________________________________________________________________
    bool Socket::waitForConnected( int msecs )
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
    void Socket::timerEvent( QTimerEvent* event )
    {

        // check timer id
        if( event->timerId() == timer_.timerId() )
        {

            #if WITH_SSH

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
    bool Socket::_tryConnect()
    {

        // Debug::Throw(0, "SSh::Socket::_tryConnect.\n" );
        if( isConnected() ) return true;

        #if WITH_SSH

        auto session( static_cast<ssh_session>(session_) );
        auto channel = static_cast<ssh_channel>( _channel() );
        if( !channel )
        {
            channel = ssh_channel_new(session);
            if( channel ) _setChannel( channel, QIODevice::ReadWrite );
            else
            {
                timer_.stop();
                setErrorString( ssh_get_error(session) );
                emit error( QAbstractSocket::ConnectionRefusedError );
                return true;
            }
        }

        auto result = ssh_channel_open_forward( channel, qPrintable( host_ ), port_, qPrintable( localHost_ ), localPort_ );
        if( result == SSH_OK )
        {

            _setConnected();
            return true;

        } else if( result != SSH_AGAIN ) {

            timer_.stop();
            setErrorString( ssh_get_error(session) );
            emit error( QAbstractSocket::ConnectionRefusedError );
            return true;

        } else return false;

        #else
        return true;
        #endif

    }

}

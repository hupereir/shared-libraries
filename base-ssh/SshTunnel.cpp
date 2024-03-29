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

#include "SshTunnel.h"
#include "Debug.h"
#include "SshSocket.h"


namespace Ssh
{
    //______________________________________________________
    Tunnel::Tunnel( QObject* parent, QTcpSocket* socket ):
        QObject( parent ),
        Counter( QStringLiteral("Ssh::Tunnel") ),
        tcpSocket_( socket ),
        sshSocket_( new Socket( this ) )
    {
        Debug::Throw( QStringLiteral("Ssh::Tunnel::Tunnel.\n") );

        buffer_.resize(maxSize_ );
        connect( tcpSocket_, &QIODevice::readyRead, this, &Tunnel::_readFromTcpSocket );
        connect( tcpSocket_, &QAbstractSocket::disconnected, this, &Tunnel::close );

        connect( sshSocket_, &Socket::connected, this, &Tunnel::_readFromTcpSocket );
        connect( sshSocket_, &QIODevice::readyRead, this, &Tunnel::_readFromSshSocket );
        connect( sshSocket_, &QIODevice::readChannelFinished, this, &Tunnel::close );

    }

    //______________________________________________________
    void Tunnel::close()
    {
        Debug::Throw( QStringLiteral("Ssh::Tunnel::close.\n") );
        tcpSocket_->close();
        sshSocket_->close();
    }

    //______________________________________________________
    void Tunnel::_readFromTcpSocket()
    {
        Debug::Throw( QStringLiteral("Ssh::Tunnel::_readFromTcpSocket.\n") );

        if( !sshSocket_->isConnected() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = tcpSocket_->bytesAvailable()) > 0 )
        {
            qint64 bytesRead = tcpSocket_->read( buffer_.data(), buffer_.size() );
            emit debug( QStringLiteral( "Ssh::Tunnel::_readFromTcpSocket - bytesAvailable=%1, bytesRead=%2" ).arg( bytesAvailable ).arg( bytesRead ) );

            if( bytesRead < 0 )
            {
                emit error( tr( "invalid read from tcp socket: %1" ).arg( bytesRead ) );
                return;
            }


            ssize_t bytesWritten = 0;
            ssize_t i = 0;
            do
            {
                i = sshSocket_->write( buffer_.data() + bytesWritten, bytesRead-bytesWritten );
                Debug::Throw() << "Ssh::Tunnel::_readFromTcpSocket - written: " << i << Qt::endl;

                if (i < 0)
                {
                    emit error( tr( "invalid write to tcp socket: %1, error: %2" ).arg( i ).arg( sshSocket_->errorString() ) );
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

        }

        return;
    }

    //______________________________________________________
    void Tunnel::_readFromSshSocket()
    {
        Debug::Throw( QStringLiteral("Ssh::Tunnel::_readFromSshSocket.\n") );

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = sshSocket_->bytesAvailable()) > 0 )
        {
            qint64 bytesRead = sshSocket_->read( buffer_.data(), buffer_.size() );
            emit debug( QStringLiteral( "Ssh::Tunnel::_readFromSshSocket - bytesAvailable=%1, bytesRead=%2" ).arg( bytesAvailable ).arg( bytesRead ) );

            if( bytesRead < 0 )
            {
                emit error( tr( "invalid read from ssh socket: %1" ).arg( bytesRead ) );
                return;
            }

            ssize_t bytesWritten = 0;
            ssize_t i = 0;
            do
            {
                i = tcpSocket_->write( buffer_.data() + bytesWritten, bytesRead - bytesWritten );
                if (i < 0)
                {
                    emit error( tr( "invalid write to tcp socket: %1" ).arg( i ) );
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

        }
        return;
    }

}

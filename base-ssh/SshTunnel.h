#ifndef SshTunnel_h
#define SshTunnel_h

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

#include "Counter.h"

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

namespace Ssh
{

    class Socket;

    //* tunnel
    class Tunnel: public QObject, private Base::Counter<Tunnel>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit Tunnel( QObject*, QTcpSocket* );

        //*@name accessors
        //@{

        QTcpSocket* tcpSocket() const
        { return tcpSocket_; }

        Socket* sshSocket() const
        { return sshSocket_; }

        //@}

        Q_SIGNALS:

        //* error message
        void error( QString );

        //* debug message
        void debug( QString );

        public Q_SLOTS:

        //* close
        void close();

        private Q_SLOTS:

        //* read from tcp socket
        void _readFromTcpSocket();

        //* read from ssh socket
        void _readFromSshSocket();

        private:

        //* tcp socket
        QTcpSocket* tcpSocket_ = nullptr;

        //* ssh socket
        Socket* sshSocket_ = nullptr;

        //* buffer
        QByteArray buffer_;

        //* max buffer size
        qint64 maxSize_ = 16384;

    };

}

#endif

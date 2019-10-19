#ifndef SshSocket_h
#define SshSocket_h

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

#include "SshBaseSocket.h"

#include <QBasicTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>
namespace Ssh
{

    //* ssh socket
    class Socket: public BaseSocket
    {

        Q_OBJECT

        public:

        //* constructor
        explicit Socket( QObject* );

        //*@name modifiers
        //@{

        //* set local port
        /* it is not really necessary but libssh asks for it, for book keeping */
        void setLocalHost( QString host, quint16 port )
        {
            localHost_ = host;
            localPort_ = port;
        }

        //* connect to host
        void connectToHost( void*, const QString&, quint16 port );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //@}

        protected:

        //* timer event
        void timerEvent( QTimerEvent* );

        private:

        //* try connect channel, returns true on success
        bool _tryConnect();

        //* session pointer
        void* session_ = nullptr;

        //* local host
        QString localHost_;

        //* port
        quint16 localPort_ = 0;

        //* host
        QString host_;

        //* port
        quint16 port_ = 0;

        //* timer
        QBasicTimer timer_;

    };

}

#endif

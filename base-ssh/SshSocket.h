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

#include "base_ssh_export.h"
#include "Counter.h"
#include "SshUtil.h"

#include <QAbstractSocket>
#include <QBasicTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>

#include <memory>

namespace Ssh
{

    //* ssh socket
    class BASE_SSH_EXPORT Socket: public QIODevice, private Base::Counter<Socket>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit Socket( QObject* );

        //* destructor
        ~Socket() override;

        //*@name accessors
        //@{

        //* true if connected
        bool isConnected() const
        { return channel_ && connected_; }

        //* true if channel is closed
        bool atEnd() const override;

        //* sequencial mode
        bool isSequential() const override
        { return true; }

        //* bytes available
        qint64 bytesAvailable() const override
        { return bytesAvailable_; }

        //@}

        //*@name modifiers
        //@{

        //* set local port
        /* it is not really necessary but libssh asks for it, for book keeping */
        void setLocalHost( const QString &host, quint16 port )
        {
            localHost_ = host;
            localPort_ = port;
        }

        //* change latency
        void setLatency( int latency )
        { latency_ = latency; }

        //* connect to host
        void connectToHost( void*, const QString&, quint16 port );

        //* close
        void close() override;

        //@}

        Q_SIGNALS:

        //* emit when connected
        int connected();

        //* error
        void error(QAbstractSocket::SocketError);

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        //* read
        qint64 readData( char*, qint64 maxSize ) override;

        //* write
        qint64 writeData( const char*, qint64 maxSize ) override;

        private:

        //* try connect channel, returns true on success
        bool _tryConnect();

        //* try read data from channel
        bool _tryRead();

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
        std::unique_ptr<void,Util::ChannelDeleter> channel_;

        //* connected
        bool connected_ = false;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 10;

        //* buffer
        QByteArray buffer_;

        //* bytes available
        qint64 bytesAvailable_ = 0;

    };

}

#endif

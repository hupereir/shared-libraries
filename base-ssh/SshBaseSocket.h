#ifndef SshBaseSocket_h
#define SshBaseSocket_h

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

#include <QAbstractSocket>
#include <QBasicTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>
namespace Ssh
{

    //* ssh socket
    class BaseSocket: public QIODevice, private Base::Counter<BaseSocket>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit BaseSocket( QObject* );

        //* destructor
        ~BaseSocket() override;

        //*@name accessors
        //@{

        //* true if connected
        bool isConnected() const
        { return channel_; }

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

        //* change latency
        void setLatency( int latency )
        { latency_ = latency; }

        //@}

        Q_SIGNALS:

        //* emit when connected
        int connected();

        //* error
        void error(QAbstractSocket::SocketError);

        public Q_SLOTS:

        //* close
        void close() override;

        protected:

        //* read
        qint64 readData( char*, qint64 maxSize ) override;

        //* write
        qint64 writeData( const char*, qint64 maxSize ) override;

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        //* get socket latency
        int _latency() const
        { return latency_; }

        //* channel
        void* _channel()
        { return channel_; }

        //* channel
        void _setChannel( void*, QIODevice::OpenMode );

        private:

        //* try connect channel, returns true on success
        bool _tryConnect();

        //* try read data from channel
        bool _tryRead();

        //* ssh channel
        void* channel_ = nullptr;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 10;

        //* buffer
        QByteArray buffer_;

        //* max buffer size
        qint64 maxSize_ = 1<<16;

        //* bytes available
        qint64 bytesAvailable_ = 0;

    };

}

#endif

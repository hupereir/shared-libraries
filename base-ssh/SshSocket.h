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
    class Socket: public QIODevice, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        Socket( QObject* );

        //* destructor
        virtual ~Socket( void );

        //*@name accessors
        //@{

        //* true if connected
        bool isConnected( void ) const
        { return channel_; }

        //* true if channel is closed
        bool atEnd( void ) const;

        //* sequencial mode
        bool isSequential( void ) const
        { return true; }

        //* bytes available
        qint64 bytesAvailable( void ) const
        { return bytesAvailable_; }

        //@}

        //*@name modifiers
        //@{

        //* connect to host
        void connectToHost( void*, const QString&, quint16 );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //@}

        Q_SIGNALS:

        //* emit when connected
        int connected( void );

        //* error
        void error(QAbstractSocket::SocketError);

        public Q_SLOTS:

        //* close
        void close( void );

        protected:

        //* read
        virtual qint64 readData( char*, qint64 maxSize );

        //* write
        virtual qint64 writeData( const char*, qint64 maxSize );

        //* timer event
        void timerEvent( QTimerEvent* );

        private:

        //* try connect channel, returns true on success
        bool _tryConnect( void );

        //* try read data from channel
        bool _tryRead( void );

        //* ssh channel
        void* channel_ = nullptr;

        //* session pointer
        void* session_;

        //* host
        QString host_;

        //* port
        quint16 port_ = 0;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 100;

        //* buffer
        QByteArray buffer_;

        //* max buffer size
        qint64 maxSize_ = 16384;

        //* bytes available
        qint64 bytesAvailable_ = 0;

    };

}

#endif

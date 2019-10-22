#ifndef SshReadFileSocket_h
#define SshReadFileSocket_h

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

#include <array>
#include <memory>

namespace Ssh
{

    //* ssh socket
    class ReadFileSocket: public QIODevice, private Base::Counter<ReadFileSocket>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ReadFileSocket( QObject* );

        //* destructor
        ~ReadFileSocket() override;

        //*@name accessors
        //@{

        //* true if connected
        bool isConnected() const
        { return sftp_ && handle_ && connected_; }

        //* true if channel is closed
        bool atEnd() const override;

        //* sequencial mode
        bool isSequential() const override
        { return true; }

        //* file size
        qint64 fileSize() const
        { return fileSize_; }

        //* bytes available
        qint64 bytesAvailable() const override
        { return bytesAvailable_; }

        //@}

        //*@name modifiers
        //@{

        //* change latency
        void setLatency( int latency )
        { latency_ = latency; }

        //* connect to remote file
        void connect( void*, const QString& );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

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

        //* timer event
        void timerEvent( QTimerEvent* );

        //* read
        qint64 readData( char*, qint64 maxSize ) override;

        //* write
        qint64 writeData( const char*, qint64 ) override
        { return -1; }

        private:

        //* try connect channel, returns true on success
        bool _tryConnect();

        //* try read data from channel
        bool _tryRead();

        //* session pointer
        void* session_ = nullptr;

        //* file
        QString remoteFileName_;

        //* ssh session
        class SftpDeleter
        {
            public:
            void operator() (void*) const;
        };
        std::unique_ptr<void,SftpDeleter> sftp_;

        //* file handle
        class FileHandleDeleter
        {
            public:
            void operator() (void*) const;
        };
        std::unique_ptr<void,FileHandleDeleter> handle_;

        //* connected
        bool connected_ = false;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 10;

        //* buffer
        QByteArray buffer_;

        //* total size
        qint64 fileSize_ = 0;

        //* bytes available
        qint64 bytesRead_ = 0;

        //* bytes available
        qint64 bytesAvailable_ = 0;

        enum
        { nParallel = 16 };

        std::array<int, nParallel> idList_;
        int currentIndex_ = 0;

    };

}

#endif

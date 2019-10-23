#ifndef SshWriteFileSocket_h
#define SshWriteFileSocket_h

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
#include "SshUtil.h"

#include <QAbstractSocket>
#include <QBasicTimer>
#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>

#include <memory>

namespace Ssh
{

    //* ssh socket
    class WriteFileSocket: public QIODevice, private Base::Counter<WriteFileSocket>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit WriteFileSocket( QObject* );

        //* destructor
        ~WriteFileSocket() override;

        //*@name accessors
        //@{

        //* true if connected
        bool isConnected() const
        { return sftp_ && handle_ && connected_; }

        //* sequencial mode
        bool isSequential() const override
        { return true; }

        //@}

        //*@name modifiers
        //@{

        //* change latency
        void setLatency( int latency )
        { latency_ = latency; }

        //* connect to remote file
        void connect( void*, const QString&, qint64 size, QFile::Permissions = 0 );

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
        qint64 readData( char*, qint64 ) override
        { return -1; }

        //* write
        qint64 writeData( const char*, qint64 maxSize ) override;

        private:

        //* try connect channel, returns true on success
        bool _tryConnect();

        //* session pointer
        void* session_ = nullptr;

        //* file
        QString remoteFileName_;

        //* ssh session
        std::unique_ptr<void,Util::SftpDeleter> sftp_;

        //* file handle
        std::unique_ptr<void,Util::FileHandleDeleter> handle_;

        //* connected
        bool connected_ = false;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 10;

        //* total size
        qint64 fileSize_ = 0;

        //* permissions
        QFile::Permissions permissions_ = 0;

    };

}

#endif

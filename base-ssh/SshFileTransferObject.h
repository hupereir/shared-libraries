#ifndef SshFileTransferObject_h
#define SshFileTransferObject_h

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
#include <QByteArray>
#include <QObject>
#include <QString>

#include <QFile>

namespace Ssh
{

    class BaseSocket;

    class FileTransferObject: public QObject, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        FileTransferObject( QObject*, QString, QString );

        //*@name modifiers
        //@{

        //* read (remote to local)
        bool read( void* );

        //* write (local to remote
        bool write( void* );

        //* wait for transfered
        /** warning, this method is blocking */
        bool waitForRead( int msecs = 30000 );

        //* wait for transfered
        /** warning, this method is blocking */
        bool waitForWritten( int msecs = 30000 );

        //@}

        //*@name accessors
        //@{

        //* file size
        qint64 fileSize( void ) const
        { return fileSize_; }

        //* bytes written
        qint64 bytesTransferred( void ) const
        { return bytesTransferred_; }

        //@}

        Q_SIGNALS:

        //* error message
        void error( QString );

        //* debug message
        void debug( QString );

        protected Q_SLOTS:

        //* process error
        void _processError( QAbstractSocket::SocketError );

        //* prepare reading
        void _prepareReading( void );

        //* read from ssh socket
        void _readFromSocket( void );

        //* write to ssh socket
        void _writeToSocket( void );

        //* close source file, once reading is finished
        void _closeSourceFile( void );

        private:

        //* source
        QString sourceFilename_;

        //* destination
        QString destinationFilename_;

        //* file size
        qint64 fileSize_ = 0;

        //* bytes read
        qint64 bytesTransferred_ = 0;

        //* source file
        QFile* sourceFile_ = nullptr;

        //* read socket
        BaseSocket* sshSocket_ = nullptr;

        //* buffer
        QByteArray buffer_;

        //* max buffer size
        qint64 maxSize_ = 16384;

    };

}

#endif

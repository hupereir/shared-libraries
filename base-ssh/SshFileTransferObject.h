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
#include "base_ssh_export.h"

#include <QAbstractSocket>
#include <QByteArray>
#include <QFile>
#include <QObject>
#include <QString>

#include <QIODevice>

namespace Ssh
{

    class BaseSocket;

    class BASE_SSH_EXPORT FileTransferObject: public QObject, private Base::Counter<FileTransferObject>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FileTransferObject( QObject*, const QString &);

        //*@name modifiers
        //@{

        //* set file size (for writing to remote)
        /** it must be called before calling ::connect */
        void setFileSize( qint64 fileSize )
        { fileSize_ = fileSize; }

        //* set file pernissions (for writing to remote)
        /** it must be called before calling ::connect */
        void setPermissions( QFile::Permissions value )
        { permissions_ = value; }

        using QObject::connect;

        //* connect to remote file
        bool connect( void*, QIODevice::OpenMode );

        //* read (remote to local)
        bool fromRemote( void*, const QString &);

        //* write (local to remote)
        bool toRemote( void*, const QString &);

        //* read (remote to local)
        bool fromRemote( void*, QIODevice* );

        //* write (local to remote)
        bool toRemote( void*, QIODevice* );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //* wait for transfered
        /** warning, this method is blocking */
        bool waitForCompleted( int msecs = 30000 );

        //@}

        //*@name accessors
        //@{

        //* file size
        qint64 fileSize() const
        { return fileSize_; }

        //* bytes written
        qint64 bytesTransferred() const
        { return bytesTransferred_; }

        //* connection state
        enum State
        {
            Uninitialized = 0,
            Connected = 1 << 0,
            Completed = 1 << 1,
            Failed = 1 << 2
        };

        Q_DECLARE_FLAGS( StateMask, State );

        //* state
        StateMask state() const
        { return state_; }

        //* true when remote file socket is connected
        bool isConnected() const
        { return state_&Connected; }

        //* true when transfer is completed
        bool isCompleted() const
        { return state_&Completed; }

        //* true when transfer has failed
        bool isFailed() const
        { return state_&Failed; }

        //* error string
        QString errorString() const
        { return error_; }

        //@}

        Q_SIGNALS:

        //* transfer percentage
        void transferred( qint64 size, qint64 transferred );

        //* error message
        void error( QString );

        //* debug message
        void debug( QString );

        //* emitted when transfer is completed
        void completed();

        //* emitted when transfer has failed
        void failed();

        private:

        //* process error
        void _processError( QAbstractSocket::SocketError );

        //* mark as connected
        void _setConnected();

        //* prepare reading
        void _prepareReading();

        //* read from ssh socket
        void _readFromSocket();

        //* write to ssh socket
        void _writeToSocket();

        //* close source file, once reading is finished
        void _closeSourceFile();

        //* close socket
        void _closeSocket();

        //* set completed
        void _setCompleted()
        {
            if( !(state_ & Completed ) )
            {
                state_ |= Completed;
                emit completed();
            }
        }

        //* set failed
        void _setFailed()
        {
            if( !(state_ & Failed) )
            {
                state_ |= StateMask(Failed|Completed);
                emit failed();
            }
        }

        //* destination
        QString remoteFileName_;

        //* file size
        qint64 fileSize_ = 0;

        //* permissions
        QFile::Permissions permissions_ = {};

        //* bytes read
        qint64 bytesTransferred_ = 0;

        //* source file
        QIODevice* localDevice_ = nullptr;

        //* true if source device is owned by us
        bool deviceOwned_ = false;

        //* read socket
        QIODevice* remoteDevice_ = nullptr;

        //* state
        StateMask state_ = Uninitialized;

        //* error string
        QString error_;

        //* buffer
        QByteArray buffer_;

    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS( Ssh::FileTransferObject::StateMask );

#endif

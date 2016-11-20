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

#include <QIODevice>

namespace Ssh
{

    class BaseSocket;

    class FileTransferObject: public QObject, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        FileTransferObject( QObject*, QString );

        //*@name modifiers
        //@{

        using QObject::connect;

        //* connect to remote file
        bool connect( void*, QIODevice::OpenMode );

        //* read (remote to local)
        bool fromRemote( void*, QString );

        //* read (remote to local)
        bool fromRemote( void*, QIODevice* );

        //* write (local to remote)
        bool toRemote( void*, QString );

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
        qint64 fileSize( void ) const
        { return fileSize_; }

        //* bytes written
        qint64 bytesTransferred( void ) const
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
        StateMask state( void ) const
        { return state_; }

        //* true when remote file socket is connected
        bool isConnected( void ) const
        { return state_&Connected; }

        //* true when transfer is completed
        bool isCompleted( void ) const
        { return state_&Completed; }

        //* true when transfer has failed
        bool isFailed( void ) const
        { return state_&Failed; }

        //* error string
        QString errorString( void ) const
        { return error_; }

        //@}

        Q_SIGNALS:

        //* transfer percentage
        void transferred( qint64 size, qint64 transferred );

        //* error message
        void error( QString );

        //* debug message
        void debug( QString );

        //* emited when transfer is completed
        void completed( void );

        //* emited when transfer has failed
        void failed( void );

        protected Q_SLOTS:

        //* process error
        void _processError( QAbstractSocket::SocketError );

        //* mark as connected
        void _setConnected( void )
        { state_ |= Connected; }

        //* prepare reading
        void _prepareReading( void );

        //* read from ssh socket
        void _readFromSocket( void );

        //* write to ssh socket
        void _writeToSocket( void );

        //* close source file, once reading is finished
        void _closeSourceFile( void );

        //* close socket
        void _closeSocket( void );

        private:

        //* set completed
        void _setCompleted( void )
        {
            if( !(state_ & Completed ) )
            {
                state_ |= Completed;
                emit completed();
            }
        }

        //* set failed
        void _setFailed( void )
        {
            if( !(state_ & Failed) )
            {
                state_ |= StateMask(Failed|Completed);
                emit failed();
            }
        }

        //* destination
        QString remoteFilename_;

        //* file size
        qint64 fileSize_ = 0;

        //* bytes read
        qint64 bytesTransferred_ = 0;

        //* source file
        QIODevice* localDevice_ = nullptr;

        //* true if source device is owned by us
        bool deviceOwned_ = false;

        //* read socket
        BaseSocket* sshSocket_ = nullptr;

        //* state
        StateMask state_ = Uninitialized;

        //* error string
        QString error_;

        //* buffer
        QByteArray buffer_;

        //* max buffer size
        qint64 maxSize_ = 16384;

    };

}

Q_DECLARE_OPERATORS_FOR_FLAGS( Ssh::FileTransferObject::StateMask );

#endif

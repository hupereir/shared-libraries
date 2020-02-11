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

#include "SshFileTransferObject.h"

#include "Debug.h"
#include "SshReadFileSocket.h"
#include "SshWriteFileSocket.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>

namespace Ssh
{

    //* max buffer size
    static const qint64 maxBufferSize = 1<<16;

    //____________________________________________________________________________
    FileTransferObject::FileTransferObject( QObject* parent, const QString &remoteFileName ):
        QObject( parent ),
        Counter( QStringLiteral("Ssh::FiletransferObject") ),
        remoteFileName_( remoteFileName )
    { buffer_.resize( maxBufferSize ); }

    //____________________________________________________________________________
    bool FileTransferObject::connect( void* session, QIODevice::OpenMode mode )
    {

        state_ = Uninitialized;
        error_.clear();

        if( mode == QIODevice::ReadOnly )
        {

            // connect to remote file and start reading
            auto socket = new ReadFileSocket( this );
            connect( socket, QOverload<QAbstractSocket::SocketError>::of( &ReadFileSocket::error ), this, &FileTransferObject::_processError );
            connect( socket, &ReadFileSocket::connected, this, &FileTransferObject::_setConnected );
            socket->connect( session, remoteFileName_ );
            remoteDevice_ = socket;
            return true;

        } else if( mode == QIODevice::WriteOnly ) {

            auto socket = new WriteFileSocket( this );
            connect( socket, QOverload<QAbstractSocket::SocketError>::of( &WriteFileSocket::error ), this, &FileTransferObject::_processError );
            connect( socket, &WriteFileSocket::connected, this, &FileTransferObject::_setConnected );
            socket->connect( session, remoteFileName_, fileSize_, permissions_ );
            remoteDevice_ = socket;
            return true;

        } else {

            emit error( error_ = tr( "incorrect open mode" ) );
            _setFailed();
            return false;

        }

    }

    //____________________________________________________________________________
    bool FileTransferObject::fromRemote( void* session, const QString &localFileName)
    {

        deviceOwned_ = true;
        auto device = new QFile( localFileName, this );

        emit debug( tr( "Transfering remote file %1 to local file %2" ).arg( remoteFileName_, localFileName ) );

        if( device->open( QIODevice::WriteOnly ) ) return fromRemote( session, device );
        else
        {
            emit error( error_ = tr( "cannot open file %1 for writting" ).arg( localFileName ) );
            _setFailed();
            return false;
        }
    }

    //____________________________________________________________________________
    bool FileTransferObject::fromRemote( void* session, QIODevice* device )
    {

        if( isFailed() ) return false;

        // store device
        localDevice_ = device;

        // create socket if needed
        auto socket = qobject_cast<ReadFileSocket*>(remoteDevice_);
        if( !remoteDevice_ ) connect( session, QIODevice::ReadOnly );
        else if( !socket )
        {
            emit error( error_ = tr( "file %1 not opended for reading" ).arg( remoteFileName_ ) );
            _setFailed();
            return false;
        }

        if( socket->isConnected() ) _prepareReading();
        else connect( socket, &ReadFileSocket::connected, this, &FileTransferObject::_prepareReading );

        connect( remoteDevice_, &QIODevice::readyRead, this, &FileTransferObject::_readFromSocket );
        connect( remoteDevice_, &QIODevice::readChannelFinished, this, &FileTransferObject::_closeSourceFile );
        connect( remoteDevice_, &QIODevice::readChannelFinished, this, &FileTransferObject::_closeSocket );

        _readFromSocket();

        return true;

    }

    //____________________________________________________________________________
    bool FileTransferObject::toRemote( void* session, const QString &localFileName )
    {

        deviceOwned_ = true;

        emit debug( tr( "Transfering local file %1 to remote file %2" ).arg( localFileName, remoteFileName_ ) );

        auto device = new QFile( localFileName, this );
        if( device->open( QIODevice::ReadOnly ) ) return toRemote( session, device );
        else
        {

            emit error( error_ = tr( "cannot open file %1 for reading" ).arg( localFileName ) );
            _setFailed();
            return false;

        }

    }

    //____________________________________________________________________________
    bool FileTransferObject::toRemote( void* session, QIODevice* device )
    {

        if( isFailed() ) return false;

        // store device
        localDevice_ = device;

        // connect to remote file
        fileSize_ = localDevice_->size();
        if( auto file = qobject_cast<QFile*>( device ) )
        { permissions_ = file->permissions(); }

        bytesTransferred_ = 0;

        // create socket if needed
        auto socket = qobject_cast<WriteFileSocket*>(remoteDevice_);
        if( !remoteDevice_ ) connect( session, QIODevice::WriteOnly );
        else if( !socket )
        {
            emit error( error_ = tr( "file %1 not opended for writing" ).arg( remoteFileName_ ) );
            _setFailed();
            return false;
        }

        if( socket->isConnected() ) _writeToSocket();
        else connect( socket, &WriteFileSocket::connected, this, &FileTransferObject::_writeToSocket );

        return true;

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForConnected( int msecs )
    {
        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::waitForConnected.\n") );

        // do nothing if socket is already closed
        if( isConnected() ) return !isFailed();

        QElapsedTimer timer;
        timer.start();

        while( ( msecs < 0 || timer.elapsed() < msecs ) && !(state_&(Connected|Failed)) )
        { qApp->processEvents(); }

        return isConnected() && !isFailed();

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForCompleted( int msecs )
    {
        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::waitForCompleted.\n") );

        // do nothing if socket is already closed
        if( isCompleted() ) return !isFailed();

        QElapsedTimer timer;
        timer.start();

        while( ( msecs < 0 || timer.elapsed() < msecs ) && !(state_&(Completed|Failed)) )
        { qApp->processEvents(); }

        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::waitForCompleted - done\n") );
        return isCompleted() && !isFailed();

    }

    //____________________________________________________________________________
    void FileTransferObject::_processError( QAbstractSocket::SocketError )
    {
        if( remoteDevice_ )
        {
            Debug::Throw() << "Ssh::FileTransferObject::_processError - " << remoteDevice_->errorString() << endl;
            emit error( error_ = remoteDevice_->errorString() );
        }

        _closeSourceFile();
        _closeSocket();
        _setFailed();

    }

    //____________________________________________________________________________
    void FileTransferObject::_setConnected()
    {
        state_ |= Connected;
        emit debug( tr( "Succesfully connected to %1" ).arg( remoteFileName_ ) );
    }

    //____________________________________________________________________________
    void FileTransferObject::_prepareReading()
    {
        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::_prepareReading.\n") );

        // store file size
        fileSize_ = qobject_cast<ReadFileSocket*>( remoteDevice_ )->fileSize();
        bytesTransferred_ = 0;
    }

    //____________________________________________________________________________
    void FileTransferObject::_readFromSocket()
    {

        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::_readFromSocket.\n") );

        // check source file and ssh socket
        if( !localDevice_->isOpen() ) return;
        if( !qobject_cast<ReadFileSocket*>(remoteDevice_)->isConnected() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = remoteDevice_->bytesAvailable()) > 0 )
        {

            auto bytesRead = remoteDevice_->read( buffer_.data(), buffer_.size() );

            if( bytesRead < 0 )
            {
                emit error( error_ = tr( "invalid read from ssh socket: %1" ).arg( bytesRead ) );
                _closeSourceFile();
                _closeSocket();
                _setFailed();
                return;
            }

            // truncate to the right number of bytes
            bytesRead = qMin( bytesRead, fileSize_ - bytesTransferred_ );
            if( !bytesRead ) continue;

            // increment
            bytesTransferred_ += bytesRead;

            // write
            ssize_t bytesWritten = 0;
            ssize_t i = 0;
            do
            {
                i = localDevice_->write( buffer_.data() + bytesWritten, bytesRead - bytesWritten );
                if (i < 0)
                {
                    emit error( error_ = tr( "invalid write to source file: %1" ).arg( i ) );
                    _closeSourceFile();
                    _closeSocket();
                    _setFailed();
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

        }

        if( fileSize_ >  0 )
        { emit transferred( fileSize_, bytesTransferred_ ); }

        if( fileSize_ == bytesTransferred_ )
        {
            _closeSourceFile();
            _closeSocket();
            _setCompleted();
        }

        return;

    }

    //______________________________________________________
    void FileTransferObject::_writeToSocket()
    {
        Debug::Throw( QStringLiteral("Ssh::FileTransferObject::_writeToSocket.\n") );


        if( !static_cast<WriteFileSocket*>(remoteDevice_)->isConnected() ) return;
        if( !localDevice_->isOpen() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = localDevice_->bytesAvailable()) > 0 )
        {

            qint64 bytesRead = localDevice_->read( buffer_.data(), buffer_.size() );
            if( bytesRead < 0 )
            {
                emit error( tr( "invalid read from source file: %1" ).arg( bytesRead ) );
                _closeSourceFile();
                _closeSocket();
                _setFailed();
                return;
            }

            // truncate to the right number of bytes
            bytesRead = qMin( bytesRead, fileSize_ - bytesTransferred_ );
            if( !bytesRead ) continue;

            ssize_t bytesWritten = 0;
            ssize_t i = 0;
            do
            {
                i = remoteDevice_->write( buffer_.data() + bytesWritten, bytesRead-bytesWritten );
                Debug::Throw() << "Ssh::FileTransferObject::_writeToSocket - written: " << i << endl;
                if (i < 0)
                {
                    emit error( tr( "invalid write to socket: %1, error: %2" ).arg( i ).arg( remoteDevice_->errorString() ) );
                    _closeSourceFile();
                    _closeSocket();
                    _setFailed();
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

            bytesTransferred_ += bytesRead;

            if( fileSize_ > 0 )
            { emit transferred( fileSize_, bytesTransferred_ ); }

        }

        _closeSourceFile();
        _closeSocket();
        _setCompleted();

        return;
    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSourceFile()
    {

        // store permissions
        auto local = qobject_cast<QFile*>( localDevice_ );
        auto remote = qobject_cast<ReadFileSocket*>( remoteDevice_ );
        if( local && remote )
        {
            Debug::Throw( QStringLiteral("Ssh::FileTransferObject::_closeSourceFile - copying permissions.\n") );
            local->setPermissions( remote->permissions() );
        }

        if( localDevice_ && deviceOwned_ )
        {
            localDevice_->close();
            deviceOwned_ = false;
        }
    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSocket()
    { if( remoteDevice_ ) remoteDevice_->close(); }

}

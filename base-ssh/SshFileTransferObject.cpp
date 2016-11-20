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
#include "SshFileReadSocket.h"
#include "SshFileWriteSocket.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QFile>

namespace Ssh
{

    //____________________________________________________________________________
    FileTransferObject::FileTransferObject( QObject* parent, QString remoteFilename ):
        QObject( parent ),
        Counter( "Ssh::FiletransferObject" ),
        remoteFilename_( remoteFilename )
    { buffer_.resize(maxSize_ ); }

    //____________________________________________________________________________
    bool FileTransferObject::connect( void* session, QIODevice::OpenMode mode )
    {

        state_ = Uninitialized;
        error_.clear();

        if( mode == QIODevice::ReadOnly )
        {

            // connect to remote file and start reading
            sshSocket_ = new FileReadSocket( this );
            connect( sshSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT( _processError(QAbstractSocket::SocketError)) );
            connect( sshSocket_, SIGNAL(connected()), this, SLOT(_setConnected()) );
            qobject_cast<FileReadSocket*>(sshSocket_)->connectToFile( session, remoteFilename_ );
            return true;

        } else if( mode == QIODevice::WriteOnly ) {

            state_ = Uninitialized;
            error_.clear();

            sshSocket_ = new FileWriteSocket( this );
            connect( sshSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT( _processError(QAbstractSocket::SocketError)) );
            connect( sshSocket_, SIGNAL(connected()), this, SLOT(_setConnected()) );
            qobject_cast<FileWriteSocket*>(sshSocket_)->connectToFile( session, remoteFilename_, fileSize_ );
            return true;

        } else {

            emit error( error_ = tr( "incorrect open mode" ) );
            _setFailed();
            return false;

        }

    }

    //____________________________________________________________________________
    bool FileTransferObject::fromRemote( void* session, QString localFilename)
    {

        Debug::Throw(0)
            << "FileTransferObject::fromRemote -"
            << " remote file: " << remoteFilename_
            << " local file: " << localFilename
            << endl;

        deviceOwned_ = true;
        QFile* device = new QFile( localFilename, this );

        if( device->open( QIODevice::WriteOnly ) ) return fromRemote( session, device );
        else
        {
            emit error( error_ = tr( "cannot open file %1 for writting" ).arg( localFilename ) );
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
        if( !sshSocket_ ) connect( session, QIODevice::ReadOnly );
        else if( !qobject_cast<FileReadSocket*>(sshSocket_) )
        {
            emit error( error_ = tr( "file %1 not opended for reading" ).arg( remoteFilename_ ) );
            _setFailed();
            return false;
        }

        if( sshSocket_->isConnected() ) _prepareReading();
        else connect( sshSocket_, SIGNAL(connected()), this, SLOT(_prepareReading()) );

        connect( sshSocket_, SIGNAL(readyRead()), this, SLOT(_readFromSocket()) );
        connect( sshSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSourceFile()) );
        connect( sshSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSocket()) );

        _readFromSocket();

        return true;

    }

    //____________________________________________________________________________
    bool FileTransferObject::toRemote( void* session, QString localFilename )
    {

        Debug::Throw(0)
            << "FileTransferObject::toRemote -"
            << " local file: " << localFilename
            << " remote file: " << remoteFilename_
            << endl;

        deviceOwned_ = true;

        QFile* device = new QFile( localFilename, this );
        if( device->open( QIODevice::ReadOnly ) ) return toRemote( session, device );
        else
        {

            emit error( error_ = tr( "cannot open file %1 for reading" ).arg( localFilename ) );
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
        bytesTransferred_ = 0;

        // create socket if needed
        if( !sshSocket_ ) connect( session, QIODevice::WriteOnly );
        else if( !qobject_cast<FileWriteSocket*>(sshSocket_) )
        {
            emit error( error_ = tr( "file %1 not opended for writing" ).arg( remoteFilename_ ) );
            _setFailed();
            return false;
        }

        if( sshSocket_->isConnected() ) _writeToSocket();
        else connect( sshSocket_, SIGNAL(connected()), this, SLOT(_writeToSocket()) );

        return true;

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForConnected( int msecs )
    {

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

        // do nothing if socket is already closed
        if( isCompleted() ) return !isFailed();

        QElapsedTimer timer;
        timer.start();

        while( ( msecs < 0 || timer.elapsed() < msecs ) && !(state_&(Completed|Failed)) )
        { qApp->processEvents(); }

        return isCompleted() && !isFailed();

    }

    //____________________________________________________________________________
    void FileTransferObject::_processError( QAbstractSocket::SocketError )
    {
        if( sshSocket_ )
        {
            Debug::Throw() << sshSocket_->errorString() << endl;
            emit error( error_ = sshSocket_->errorString() );
        }

        _closeSourceFile();
        _closeSocket();
        _setFailed();

    }

    //____________________________________________________________________________
    void FileTransferObject::_prepareReading( void )
    {
        Debug::Throw( "Ssh::FileTransferObject::_prepareReading.\n" );

        // store file size
        fileSize_ = qobject_cast<FileReadSocket*>( sshSocket_ )->fileSize();
        bytesTransferred_ = 0;
    }

    //____________________________________________________________________________
    void FileTransferObject::_readFromSocket( void )
    {

        Debug::Throw( "Ssh::FileTransferObject::_readFromSocket.\n" );

        // check source file and ssh socket
        if( !localDevice_->isOpen() ) return;
        if( !sshSocket_->isConnected() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = sshSocket_->bytesAvailable()) > 0 )
        {

            qint64 bytesRead = sshSocket_->read( buffer_.data(), buffer_.size() );
            emit debug( QString( "Ssh::FileTransferObject::_readFromSocket - bytesAvailable=%1, bytesRead=%2" ).arg( bytesAvailable ).arg( bytesRead ) );

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

        Debug::Throw(0) << "Ssh::FileTransferObject::_readFromSocket - read " << 100*bytesTransferred_/fileSize_ << "%" << endl;
        emit transferred( fileSize_, bytesTransferred_ );
        if( fileSize_ == bytesTransferred_ )
        {
            _closeSourceFile();
            _closeSocket();
            _setCompleted();
        }

        return;

    }

    //______________________________________________________
    void FileTransferObject::_writeToSocket( void )
    {
        Debug::Throw( "Ssh::FileTransferObject::_writeToSocket.\n" );

        if( !sshSocket_->isConnected() ) return;
        if( !localDevice_->isOpen() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = localDevice_->bytesAvailable()) > 0 )
        {

            qint64 bytesRead = localDevice_->read( buffer_.data(), buffer_.size() );
            emit debug( QString( "Ssh::FileTransferObject::_writeToSocket - bytesAvailable=%1, bytesRead=%2" ).arg( bytesAvailable ).arg( bytesRead ) );

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
                i = sshSocket_->write( buffer_.data() + bytesWritten, bytesRead-bytesWritten );
                Debug::Throw() << "Ssh::Tunnel::_readFromTcpSocket - written: " << i << endl;
                if (i < 0)
                {
                    emit error( tr( "invalid write to tcp socket: %1, error: %2" ).arg( i ).arg( sshSocket_->errorString() ) );
                    _closeSourceFile();
                    _closeSocket();
                    _setFailed();
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

            bytesTransferred_ += bytesRead;

            Debug::Throw(0) << "Ssh::FileTransferObject::_writeToSocket - wrote " << 100*bytesTransferred_/fileSize_ << "%" << endl;
            emit transferred( fileSize_, bytesTransferred_ );
        }

        _closeSourceFile();
        _closeSocket();
        _setCompleted();

        return;
    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSourceFile( void )
    {
        if( localDevice_ && deviceOwned_ )
        {
            localDevice_->close();
            deviceOwned_ = false;
        }
    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSocket( void )
    { if( sshSocket_ ) sshSocket_->close(); }

}

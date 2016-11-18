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

namespace Ssh
{

    //____________________________________________________________________________
    FileTransferObject::FileTransferObject( QObject* parent, QString source, QString destination ):
        QObject( parent ),
        Counter( "Ssh::FiletransferObject" ),
        sourceFilename_( source ),
        destinationFilename_( destination ),
        sourceFile_( new QFile( sourceFilename_, this ) ),
        sshSocket_( new FileWriteSocket( this ) )
    {
        buffer_.resize(maxSize_ );

        connect( sshSocket_, SIGNAL(connected()), this, SLOT(_writeToSocket()) );

    }

    //____________________________________________________________________________
    bool FileTransferObject::read( void* session )
    {

        state_ = Uninitialized;
        error_.clear();

        // create source file
        if( !sourceFile_->open( QIODevice::WriteOnly ) )
        {
            emit error( error_ = tr( "cannot open file %1 for writting" ).arg( sourceFilename_ ) );
            _setFailed();
            return false;
        }

        // connect to remote file and start reading
        sshSocket_ = new FileReadSocket( this );

        connect( sshSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT( _processError(QAbstractSocket::SocketError)) );
        connect( sshSocket_, SIGNAL(connected()), this, SLOT(_prepareReading()) );
        connect( sshSocket_, SIGNAL(readyRead()), this, SLOT(_readFromSocket()) );
        connect( sshSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSourceFile()) );
        connect( sshSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSocket()) );

        qobject_cast<FileReadSocket*>(sshSocket_)->connectToFile( session, destinationFilename_ );
        return true;

    }

    //____________________________________________________________________________
    bool FileTransferObject::write( void* session )
    {

        state_ = Uninitialized;

        if( !sourceFile_->open( QIODevice::ReadOnly ) )
        {
            emit error( error_ = tr( "cannot open file %1 for reading" ).arg( sourceFilename_ ) );
            _setFailed();
            return false;
        }

        // connect to remote file
        fileSize_ = sourceFile_->size();
        bytesTransferred_ = 0;

        sshSocket_ = new FileWriteSocket( this );
        connect( sshSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT( _processError(QAbstractSocket::SocketError)) );
        connect( sshSocket_, SIGNAL(connected()), this, SLOT(_writeToSocket()) );
        qobject_cast<FileWriteSocket*>(sshSocket_)->connectToFile( session, destinationFilename_, fileSize_ );

        return true;

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForTransferred( int msecs )
    {

        // do nothing if socket is already closed
        if( isCompleted() ) return !isFailed();

        QElapsedTimer timer;
        timer.start();

        while( ( msecs < 0 || timer.elapsed() < msecs ) && !(state_&Completed) )
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
        state_ |= Connected;
        fileSize_ = qobject_cast<FileReadSocket*>( sshSocket_ )->fileSize();
        bytesTransferred_ = 0;
    }

    //____________________________________________________________________________
    void FileTransferObject::_readFromSocket( void )
    {

        Debug::Throw( "Ssh::FileTransferObject::_readFromSocket.\n" );

        // check source file and ssh socket
        if( !sourceFile_->isOpen() ) return;
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
                i = sourceFile_->write( buffer_.data() + bytesWritten, bytesRead - bytesWritten );
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
        if( !sourceFile_->isOpen() ) return;

        state_ |= Connected;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = sourceFile_->bytesAvailable()) > 0 )
        {

            qint64 bytesRead = sourceFile_->read( buffer_.data(), buffer_.size() );
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
    { if( sourceFile_ ) sourceFile_->close(); }

    //____________________________________________________________________________
    void FileTransferObject::_closeSocket( void )
    { if( sshSocket_ ) sshSocket_->close(); }

}

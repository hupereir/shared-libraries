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

        // create source file
        if( !sourceFile_->open( QIODevice::WriteOnly ) )
        {
            Debug::Throw(0) << "FileTransferObject::read - could not open file " << sourceFilename_ << " for writting" << endl;
            return false;
        }

        // connect to remote file and start reading
        sshSocket_ = new FileReadSocket( this );

        connect( sshSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT( _processError(QAbstractSocket::SocketError)) );
        connect( sshSocket_, SIGNAL(connected()), this, SLOT(_prepareReading()) );
        connect( sshSocket_, SIGNAL(readyRead()), this, SLOT(_readFromSocket()) );
        connect( sshSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSourceFile()) );

        qobject_cast<FileReadSocket*>(sshSocket_)->connectToFile( session, destinationFilename_ );
        return true;

    }

    //____________________________________________________________________________
    bool FileTransferObject::write( void* session )
    {

        if( !sourceFile_->open( QIODevice::ReadOnly ) )
        {
            Debug::Throw(0) << "FileTransferObject::write - could not open file " << sourceFilename_ << " for reading" << endl;
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
    bool FileTransferObject::waitForRead( int msecs )
    {

        // do nothing if socket is already closed
        if( sshSocket_->isConnected() && sshSocket_->atEnd() ) return true;

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        {
            qApp->processEvents();
            if( sshSocket_->isConnected() && sshSocket_->atEnd() ) break;
        }

        // return sshSocket_->isConnected() && sshSocket_->atEnd() &&
        return sshSocket_->isConnected() && fileSize_ == bytesTransferred_;

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForWritten( int msecs )
    {

        // do nothing if socket is already closed
        if( sshSocket_->isConnected() && sourceFile_->atEnd() ) return true;

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        {
            qApp->processEvents();
            if( sshSocket_->isConnected() && sourceFile_->atEnd() ) break;
        }

        // return sshSocket_->isConnected() && sshSocket_->atEnd() &&
        return !sshSocket_->isConnected() && fileSize_ == bytesTransferred_;

    }

    //____________________________________________________________________________
    void FileTransferObject::_processError( QAbstractSocket::SocketError )
    {  if( sshSocket_ ) Debug::Throw() << sshSocket_->errorString() << endl; }

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
                emit error( tr( "invalid read from ssh socket: %1" ).arg( bytesRead ) );
                _closeSourceFile();
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
                    emit error( tr( "invalid write to source file: %1" ).arg( i ) );
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

        }

        Debug::Throw(0) << "Ssh::FileTransferObject::_readFromSocket - read " << 100*bytesTransferred_/fileSize_ << "%" << endl;

        return;

    }

    //______________________________________________________
    void FileTransferObject::_writeToSocket( void )
    {
        Debug::Throw( "Ssh::FileTransferObject::_writeToSocket.\n" );

        if( !sshSocket_->isConnected() ) return;
        if( !sourceFile_->isOpen() ) return;

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
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

            bytesTransferred_ += bytesRead;

            Debug::Throw(0) << "Ssh::FileTransferObject::_writeToSocket - wrote " << 100*bytesTransferred_/fileSize_ << "%" << endl;
        }

        _closeSourceFile();
        sshSocket_->close();

        return;
    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSourceFile( void )
    {

        Debug::Throw( "Ssh::FileTransferObject::_closeSourceFile.\n" );
        sourceFile_->close();

    }

}

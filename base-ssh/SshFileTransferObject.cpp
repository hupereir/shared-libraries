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
        fileReadSocket_( new FileReadSocket( this ) ),
        fileWriteSocket_( new FileWriteSocket( this ) )
    {
        buffer_.resize(maxSize_ );

        // connect
        connect( fileReadSocket_, SIGNAL(connected()), this, SLOT(_prepareReading()) );
        connect( fileReadSocket_, SIGNAL(readyRead()), this, SLOT(_readFromSocket()) );
        connect( fileReadSocket_, SIGNAL(readChannelFinished()), this, SLOT(_closeSourceFile()) );
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

        // start reading
        fileReadSocket_->connectToFile( session, destinationFilename_ );
        return true;

    }

    //_______________________________________________________________________
    bool FileTransferObject::waitForTransfered( int msecs )
    {

        // do nothing if socket is already closed
        if( fileReadSocket_->isConnected() && fileReadSocket_->atEnd() ) return true;

        QElapsedTimer timer;
        timer.start();

        while( msecs < 0 || timer.elapsed() < msecs )
        {
            qApp->processEvents();
            if( fileReadSocket_->isConnected() && fileReadSocket_->atEnd() ) break;
        }

        // return fileReadSocket_->isConnected() && fileReadSocket_->atEnd() &&
        return fileReadSocket_->isConnected() && fileSize_ == bytesRead_;

    }

    //____________________________________________________________________________
    void FileTransferObject::_prepareReading( void )
    {
        Debug::Throw( "Ssh::FileTransferObject::_prepareReading.\n" );

        // store file size
        fileSize_ = fileReadSocket_->fileSize();
        bytesRead_ = 0;

    }

    //____________________________________________________________________________
    void FileTransferObject::_readFromSocket( void )
    {

        Debug::Throw( "Ssh::FileTransferObject::_readFromSocket.\n" );

        // make sure source file is open
        if( !sourceFile_->isOpen() ) return;

        // read from socket, write through ssh
        qint64 bytesAvailable = 0;
        while( (bytesAvailable = fileReadSocket_->bytesAvailable()) > 0 )
        {

            qint64 bytesRead = fileReadSocket_->read( buffer_.data(), buffer_.size() );
            if( bytesRead < 0 )
            {
                _closeSourceFile();
                return;
            }

            // truncate to the right number of bytes
            bytesRead = qMin( bytesRead, fileSize_ - bytesRead_ );
            if( !bytesRead ) continue;

            // increment
            bytesRead_ += bytesRead;

            // write
            ssize_t bytesWritten = 0;
            ssize_t i = 0;
            do
            {
                i = sourceFile_->write( buffer_.data() + bytesWritten, bytesRead - bytesWritten );
                if (i < 0)
                {
                    emit error( tr( "invalid write source file: %1" ).arg( i ) );
                    return;
                }

                bytesWritten += i;

            } while( i > 0 && bytesWritten < bytesRead );

        }

        Debug::Throw(0) << "Ssh::FileTransferObject::_readFromSocket - read " << 100*bytesRead_/fileSize_ << "%" << endl;

        return;

    }

    //____________________________________________________________________________
    void FileTransferObject::_closeSourceFile( void )
    {

        Debug::Throw( "Ssh::FileTransferObject::_closeSourceFile.\n" );
        sourceFile_->close();

    }

}

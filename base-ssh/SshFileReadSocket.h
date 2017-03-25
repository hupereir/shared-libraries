#ifndef SshFileReadSocket_h
#define SshFileReadSocket_h

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

#include "SshBaseSocket.h"
#include "TimeStamp.h"

#include <QBasicTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>

#include <sys/stat.h>

namespace Ssh
{

    //* ssh socket
    class FileReadSocket: public BaseSocket
    {

        Q_OBJECT

        public:

        //* constructor
        FileReadSocket( QObject* );

        //*@name modifiers
        //@{

        //* connect to remote
        void connectToFile( void*, const QString& );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //@}

        //*@name accessors
        //@{

        //* file size
        qint64 fileSize( void ) const
        { return isConnected() ? stat_.st_size:0; }

        //* access time
        TimeStamp accessTime( void ) const
        { return isConnected() ? TimeStamp( stat_.st_atime ):TimeStamp(); }

        //* modification time
        TimeStamp modificationTime( void ) const
        { return isConnected() ? TimeStamp( stat_.st_mtime ):TimeStamp(); }

        //@}

        protected:

        //* timer event
        void timerEvent( QTimerEvent* );

        private:

        //* try connect channel, returns true on success
        bool _tryConnect( void );

        //* session pointer
        void* session_ = nullptr;

        //* host
        QString path_;

        //* file information
        struct ::stat stat_;

        //* timer
        QBasicTimer timer_;

    };

}

#endif

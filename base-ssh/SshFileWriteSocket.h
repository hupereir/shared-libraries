#ifndef SshFileWriteSocket_h
#define SshFileWriteSocket_h

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

#include <QBasicTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QIODevice>
#include <QTimerEvent>

#include <sys/stat.h>

namespace Ssh
{

    //* ssh socket
    class FileWriteSocket: public BaseSocket
    {

        Q_OBJECT

        public:

        //* constructor
        FileWriteSocket( QObject* );

        //* destructor
        virtual ~FileWriteSocket( void ) = default;

        //*@name modifiers
        //@{

        //* connect to remote
        void connectToFile( void*, const QString&, quint64 size, int mode = 0644  );

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //@}

        protected:

        //* timer event
        void timerEvent( QTimerEvent* );

        private:

        //* try connect channel, returns true on success
        bool _tryConnect( void );

        //* session pointer
        void* session_ = nullptr;

        //* filename
        QString path_;

        //* size
        quint64 size_ = 0;

        //* file information
        int mode_ = 0;

        //* timer
        QBasicTimer timer_;

        //* latency
        const int latency_ = 100;

    };

}

#endif

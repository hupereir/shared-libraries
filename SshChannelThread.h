#ifndef SshChannelThread_h
#define SshChannelThread_h

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
#include "SshTunnelAttributes.h"

#include <QMutex>
#include <QThread>

namespace Ssh
{

    //* tunnel
    class ChannelThread: public QThread, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        ChannelThread( QObject*, const TunnelAttributes&, void* session, int socket );

        //* destructor
        virtual ~ChannelThread( void );

        //*@name accessors
        //@{

        //* tunnel attributes
        const TunnelAttributes& attributes( void ) const
        { return attributes_; }

        //* socket
        int socket( void ) const
        { return socket_; }

        //@}

        //*@name modifiers
        //@{

        //* assign mutex
        void setMutex( QMutex* mutex )
        { mutex_ = mutex; }

        //@}

        Q_SIGNALS:

        //* error message
        void error( QString );

        //* debug message
        void debug( QString );

        public Q_SLOTS:

        //*@name modifiers
        //@{

        //* close listening socket
        void close( void );

        //@}

        protected:

        //* listen to connections to socket
        virtual void run( void );

        private:

        //* tunnel attributes
        TunnelAttributes attributes_;

        //* ssh session
        void* session_ = nullptr;

        //* ssh channel
        void* channel_ = nullptr;

        //* mutex
        QMutex* mutex_ = nullptr;

        //* socket
        int socket_ = -1;

    };

}

#endif

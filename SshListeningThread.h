#ifndef SshListeningThread_h
#define SshListeningThread_h

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

#include <QThread>

namespace Ssh
{

    //* tunnel
    class ListeningThread: public QThread, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        ListeningThread( QObject*, const TunnelAttributes& );

        //* destructor
        virtual ~ListeningThread( void );

        //*@name accessors
        //@{

        //* tunnel attributes
        const TunnelAttributes& attributes( void )
        { return attributes_; }

        //@}

        Q_SIGNALS:

        //* error message
        void error( QString );

        //* new connection is available
        void newConnection( int port, int socket );

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

        //* socket
        int socket_ = -1;

    };

}

#endif

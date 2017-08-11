#ifndef Client_h
#define Client_h

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
#include "BaseSocketInterface.h"
#include "ServerCommand.h"

#include <QTcpSocket>

namespace Server
{

    //* interprocess communication client
    class Client : public BaseSocketInterface, private Base::Counter<Client>
    {

        //* Qt meta object macro
        Q_OBJECT

        public:

        //* client list
        using List = QList<Client*>;

        //* constructor
        explicit Client( QObject*, QTcpSocket* = nullptr );

        //* id
        quint32 id() const
        { return id_; }

        /** returns true if message could be sent */
        void sendCommand( const ServerCommand& );

        //* used to retrieve client matching id
        class SameIdFTor
        {

            public:

            //* constructor
            explicit SameIdFTor( quint32 id ):
                id_( id )
            {}

            //* predicate
            template<class T>
            bool operator() ( const T& client ) const
            { return client->id() == id_; }

            protected:

            //* prediction
            quint32 id_;

        };

        Q_SIGNALS:

        //* emitted when a message is available
        void commandAvailable( Server::ServerCommand );

        private Q_SLOTS:

        //* process buffer
        void _parseBuffer( qint32, QByteArray );

        private:

        //* client counter
        static quint32& _counter();

        //* client id
        quint32 id_ = 0;

    };

};

#endif

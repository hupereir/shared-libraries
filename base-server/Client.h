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

#include "BaseSocketInterface.h"
#include "Counter.h"
#include "Functors.h"
#include "ServerCommand.h"
#include "base_server_export.h"

#include <QTcpSocket>

namespace Server
{

    //* interprocess communication client
    class BASE_SERVER_EXPORT Client : public BaseSocketInterface, private Base::Counter<Client>
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
        using SameIdFTor = Base::Functor::Unary<Client, quint32, &Client::id>;

        Q_SIGNALS:

        //* emitted when a message is available
        void commandAvailable( Server::ServerCommand );

        private:

        //* process buffer
        void _parseBuffer( qint32, QByteArray );

        //* client counter
        static quint32& _counter();

        //* client id
        quint32 id_ = 0;

    };

};

#endif

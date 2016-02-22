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
#include "ServerCommand.h"

#include <QTcpSocket>

namespace Server
{

    //* interprocess communication client
    class Client : public BaseSocketInterface
    {

        //* Qt meta object macro
        Q_OBJECT

        public:

        //* client list
        using List = QList<Client*>;

        //* constructor
        Client( QObject* parent, QTcpSocket* socket );

        //* destructor
        virtual ~Client( void ) = default;

        //* id
        int id( void ) const
        { return id_; }

        /** returns true if message could be sent */
        bool sendCommand( const ServerCommand& );

        //* used to retrieve client matching id
        class SameIdFTor
        {

            public:

            //* constructor
            SameIdFTor( int id ):
                id_( id )
            {}

            //* destructor
            virtual ~SameIdFTor( void )
            {}

            //* predicate
            virtual bool operator() ( const Client* client ) const
            { return client->id() == id_; }

            protected:

            //* prediction
            int id_;

        };

        Q_SIGNALS:

        //* emitted when a message is available
        void commandAvailable( Server::ServerCommand );

        private Q_SLOTS:

        //* send all commands
        virtual void _sendCommands( void );

        //* process buffer
        virtual void _parseBuffer( qint32, QByteArray );

        private:

        //* client counter
        static int& _counter( void );

        //* client id
        int id_ = 0;

        //* messages
        using CommandList = QList<ServerCommand>;

        //* commands
        CommandList commands_;

    };

};

#endif

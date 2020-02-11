#ifndef ApplicationManager_h
#define ApplicationManager_h

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

#include "base_server_export.h"
#include "Client.h"
#include "CommandLineArguments.h"
#include "CommandLineParser.h"
#include "Counter.h"
#include "ObjectDeleter.h"
#include "ServerCommand.h"

#include <QBasicTimer>
#include <QHash>
#include <QHostAddress>
#include <QList>
#include <QObject>
#include <QPair>
#include <QTcpServer>
#include <QTimerEvent>

#include <memory>

namespace Server
{

    //* ensures only one instance of an application is running
    class BASE_SERVER_EXPORT ApplicationManager: public QObject, private Base::Counter<ApplicationManager>
    {

        //* Qt meta object macro
        Q_OBJECT

        public:

        //* constructor
        explicit ApplicationManager( QObject* );

        //*@name accessors
        //@{

        //* commandLine parser
        static CommandLineParser commandLineParser( const CommandLineArguments& = CommandLineArguments(), bool ignoreWarnings = true );

        //* retrieve Application ID
        const ApplicationId& id() const
        { return id_; }

        //* client
        bool hasClient() const
        { return static_cast<bool>(client_); }

        //* client
        Client& client() const
        { return *client_; }

        //@}

        //*@name modifiers
        //@{

        //* application name
        void setApplicationName( const QString& );

        //* application state enumeration
        enum class State
        {

            StateUnknown,
            AwaitingReply,
            Alive,
            Dead
        };

        //* changes application state, emit signal if changed
        bool setState( Server::ApplicationManager::State state )
        {
            if( state == state_ ) return false;
            state_ = state;
            return true;
        }

        //@}

        //* (re)initialize server/client connections
        void initialize( const CommandLineArguments &args = CommandLineArguments() );

        Q_SIGNALS:

        //* emitted when a given command is recieved
        void commandRecieved( Server::ServerCommand );

        //* emitted when the client is (re)initialized
        void initialized();

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        private:

        //* a new connection is granted
        void _newConnection();

        //* a connection was closed
        void _serverConnectionClosed();

        //* a connection was closed
        void _clientConnectionClosed();

        //* client recieves errors
        void _error( QAbstractSocket::SocketError );

        //* redistribute message when a connected client sends one
        void _redirect( const Server::ServerCommand &);

        //* reads signal from server
        void _process( const Server::ServerCommand &);

        //* start timeout
        void _startTimer();

        //* client pointer
        using ClientPtr = std::shared_ptr<Client>;

        //* pair of application id and client
        using ClientPair = QPair<ApplicationId, ClientPtr>;

        //* map of clients
        using ClientMap = QHash<ApplicationId, ClientPtr>;

        //* list of clients
        using ClientList = QList<ClientPtr>;

        //* used to retrieve clients for a given state
        class BASE_SERVER_EXPORT SameStateFTor: public Client::SameStateFTor
        {
            public:

            //* constructor
            template<class T>
            explicit SameStateFTor( const T& state ):
                Client::SameStateFTor( state )
            {}

            //* parent class operator
            using Client::SameStateFTor::operator();

            //* predicate
            bool operator() ( const ClientPair& pair ) const
            { return Client::SameStateFTor::operator() (pair.second); }

        };

        /** \brief register a client, returns true if application is new.
        if forced is set to true, the old cliend, if any, is replaced
        */
        ClientMap::iterator _register( const ApplicationId&, ClientPtr, bool forced = false );

        //* redirect message
        void _redirect( const ServerCommand&, ClientPtr );

        //* broadcast a message to all registered clients but the sender (if valid)
        void _broadcast( const ServerCommand&, ClientPtr sender = ClientPtr() );

        //* initialize client
        bool _initializeClient();

        //* initialize server
        bool _initializeServer();

        //* host
        QHostAddress host_ = QHostAddress::LocalHost;

        //* port
        int port_ = 8091;

        //* arguments
        CommandLineArguments arguments_;

        //* Server
        std::unique_ptr<QTcpServer, Base::ObjectDeleter> server_;

        //* true if initializeServer was called
        bool serverInitialized_ = false;

        //* Client
        std::unique_ptr<Client, Base::ObjectDeleter> client_;

        //* list of connected clients
        ClientList connectedClients_;

        //* maps accepted clients and amount of request
        ClientMap acceptedClients_;

        //* application name
        ApplicationId id_;

        //* manager status
        State state_ = State::AwaitingReply;

        //* reply timeout
        QBasicTimer timer_;

    };
};

#endif

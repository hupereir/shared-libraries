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

#include "Client.h"
#include "CommandLineArguments.h"
#include "CommandLineParser.h"
#include "Counter.h"
#include "ServerCommand.h"

#include <QBasicTimer>
#include <QHostAddress>
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QTcpServer>
#include <QTimerEvent>

namespace Server
{

    //* ensures only one instance of an application is running
    class ApplicationManager: public QObject, private Base::Counter<ApplicationManager>
    {

        //* Qt meta object macro
        Q_OBJECT

        public:

        //* constructor
        explicit ApplicationManager( QObject* );

        //* destructor
        ~ApplicationManager( void ) override;

        //* application name
        void setApplicationName( const QString& );

        //* commandLine parser
        static CommandLineParser commandLineParser( CommandLineArguments = CommandLineArguments(), bool ignoreWarnings = true );

        //* application state enumeration
        enum State
        {

            StateUnknown,
            AwaitingReply,
            Alive,
            Dead
        };

        //* changes application state, emit signal if changed
        bool setState( const State& state )
        {
            if( state == state_ ) return false;
            state_ = state;
            return true;
        }

        //* reference to "this" client
        Client& client( void ) const
        { return *client_; }

        //* retrieve Application ID
        const ApplicationId& id( void ) const
        { return id_; }

        public Q_SLOTS:

        //* (re)initialize server/client connections
        void initialize( CommandLineArguments args = CommandLineArguments() );

        Q_SIGNALS:

        //* emitted when a given command is recieved
        void commandRecieved( Server::ServerCommand );

        //* emitted when the server is (re)initialized
        void initialized( void );

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        //* pair of application id and client
        using ClientPair = QPair< ApplicationId, Client* >;

        //* map of clients
        using ClientMap = QMap< ApplicationId, Client* >;

        //* list of clients
        using ClientList = QList< Client* >;

        //* used to retrieve clients for a given state
        class SameStateFTor: public Client::SameStateFTor
        {
            public:

            //* constructor
            explicit SameStateFTor( QAbstractSocket::SocketState state ):
                Client::SameStateFTor( state )
            {}

            //* predicate
            bool operator() ( const ClientPair& pair ) const
            { return Client::SameStateFTor::operator() (pair.second); }

            //* predicate
            bool operator() ( const Client* client ) const
            { return Client::SameStateFTor::operator() (client); }

        };

        //* used to retrieve pair with matching client
        class SameClientFTor
        {
            public:

            //* constructor
            explicit SameClientFTor( Client* client ):
                client_( client )
                {}

            //* predicate
            bool operator() ( const ClientPair& pair ) const
            { return pair.second == client_; }

            //* predicate
            bool operator() ( Client* client ) const
            { return client == client_; }

            private:

            //* prediction
            Client* client_;
        };

        /** \brief register a client, returns true if application is new.
        if forced is set to true, the old cliend, if any, is replaced
        */
        ClientMap::iterator _register( const ApplicationId& id, Client* client, bool forced = false );

        //* redirect message
        void _redirect( ServerCommand, Client* );

        //* broadcast a message to all registered clients but the sender (if valid)
        void _broadcast( ServerCommand, Client* sender = 0 );

        protected Q_SLOTS:

        //* a new connection is granted
        void _newConnection( void );

        //* a connection was closed
        void _serverConnectionClosed( void );

        //* a connection was closed
        void _clientConnectionClosed( void );

        //* client recieves errors
        void _error( QAbstractSocket::SocketError );

        //* redistribute message when a connected client sends one
        void _redirect( Server::ServerCommand );

        //* reads signal from server
        void _process( Server::ServerCommand );

        //* start timeout
        void _startTimer( void );

        private:

        //* initialize client
        bool _initializeClient( void );

        //* initialize server
        bool _initializeServer( void );

        //* host
        QHostAddress host_;

        //* port
        int port_ = 0;

        //* arguments
        CommandLineArguments arguments_;

        //* Server
        QTcpServer* server_ = nullptr;

        //* true if initializeServer was called
        bool serverInitialized_ = false;

        //* Client
        Client* client_ = nullptr;

        //* list of connected clients
        ClientList connectedClients_;

        //* maps accepted clients and amount of request
        ClientMap acceptedClients_;

        //* application name
        ApplicationId id_;

        //* manager status
        State state_ = StateUnknown;

        //* reply timeout
        QBasicTimer timer_;

    };
};

#endif

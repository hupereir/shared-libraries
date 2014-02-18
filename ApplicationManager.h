#ifndef ApplicationManager_h
#define ApplicationManager_h

// $Id$

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
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QTimerEvent>

#include <QTcpServer>
#include <QHostAddress>

namespace Server
{

    //! ensures only one instance of an application is running
    class ApplicationManager: public QObject, public Counter
    {

        //! Qt meta object macro
        Q_OBJECT

        public:

        //! constructor
        ApplicationManager( QObject* parent );

        //! destructor
        virtual ~ApplicationManager( void );

        //! application name
        virtual void setApplicationName( const QString& name );

        //! commandLine parser
        static CommandLineParser commandLineParser( CommandLineArguments = CommandLineArguments(), bool ignoreWarnings = true );

        //! application state enumeration
        enum State
        {

            StateUnknown,
            AwaitingReply,
            Alive,
            Dead
        };

        //! changes application state, emit signal if changed
        virtual bool setState( const State& state )
        {
            if( state == state_ ) return false;
            state_ = state;
            return true;
        }

        //! reference to "this" client
        virtual Client& client( void ) const
        { return *client_; }

        //! retrieve Application ID
        virtual const ApplicationId& id( void ) const
        { return id_; }

        public Q_SLOTS:

        //! (re)initialize server/client connections
        virtual void initialize( CommandLineArguments args = CommandLineArguments() );

        Q_SIGNALS:

        //! emitted when manager state is changed
        // void stateChanged( Server::ApplicationManager::State state );

        //! emitted when manager is ALIVE and request is recieved
        // void serverRequest( const CommandLineArguments& args );

        //! emitted when a given command is recieved
        void commandRecieved( Server::ServerCommand );

        //! emitted when the server is (re)initialized
        void initialized( void );

        protected:

        //! timer event
        virtual void timerEvent( QTimerEvent* );

        //! reference to server
        virtual QTcpServer& _server() const
        { return *server_; }

        //! pair of application id and client
        typedef QPair< ApplicationId, Client* > ClientPair;

        //! map of clients
        typedef QMap< ApplicationId, Client* > ClientMap;

        //! list of clients
        typedef QList< Client* > ClientList;

        //! used to retrieve clients for a given state
        class SameStateFTor: public Client::SameStateFTor
        {
            public:

            //! constructor
            SameStateFTor( QAbstractSocket::SocketState state ):
                Client::SameStateFTor( state )
            {}

            //! destructor
            virtual ~SameStateFTor( void )
            {}

            //! predicate
            bool operator() ( const ClientPair& pair ) const
            { return Client::SameStateFTor::operator() (pair.second); }

            //! predicate
            bool operator() ( const Client* client ) const
            { return Client::SameStateFTor::operator() (client); }

        };

        //! map of accepted clients
        ClientMap& _acceptedClients( void )
        { return acceptedClients_; }

        //! list of connected clients
        ClientList& _connectedClients( void )
        { return connectedClients_; }

        /*! \brief register a client, returns true if application is new.
        if forced is set to true, the old cliend, if any, is replaced
        */
        virtual Client* _register( const ApplicationId& id, Client* client, bool forced = false );

        //! redirect message
        virtual void _redirect( ServerCommand, Client* );

        //! broadcast a message to all registered clients but the sender (if valid)
        virtual void _broadcast( ServerCommand, Client* sender = 0 );

        protected Q_SLOTS:

        //! a new connection is granted
        virtual void _newConnection( void );

        //! a connection was closed
        virtual void _serverConnectionClosed( void );

        //! a connection was closed
        virtual void _clientConnectionClosed( void );

        //! client recieves errors
        virtual void _error( QAbstractSocket::SocketError );

        //! redistribute message when a connected client sends one
        virtual void _redirect( Server::ServerCommand );

        //! reads signal from server
        void _process( Server::ServerCommand );

        //! start timeout
        void _startTimer( void );

        private:

        //! host
        const QHostAddress& _host( void ) const
        { return host_; }

        //! host
        void _setHost( const QHostAddress& host )
        { host_ = host; }

        //! port
        const unsigned int& _port( void ) const
        { return port_; }

        //! port
        void _setPort( const unsigned int& port )
        { port_ = port; }

        //! arguments
        const CommandLineArguments& _arguments( void ) const
        { return arguments_; }

        //! arguments
        void _setArguments( const CommandLineArguments& arguments )
        { arguments_ = arguments; }

        // initialize client
        bool _initializeClient( void );

        //! initialize server
        bool _initializeServer( void );

        //! server initialization
        bool _serverInitialized( void ) const
        { return serverInitialized_; }

        //! server initialization
        void _setServerInitialized( bool value )
        { serverInitialized_ = value; }

        // host
        QHostAddress host_;

        // port
        unsigned int port_;

        // arguments
        CommandLineArguments arguments_;

        //! Server
        QTcpServer* server_;

        //! true if initializeServer was called
        bool serverInitialized_;

        //! Client
        Client* client_;

        //! list of connected clients
        ClientList connectedClients_;

        //! maps accepted clients and amount of request
        ClientMap acceptedClients_;

        //! application name
        ApplicationId id_;

        //! manager status
        State state_;

        //! reply timeout
        QBasicTimer timer_;

    };
};

#endif

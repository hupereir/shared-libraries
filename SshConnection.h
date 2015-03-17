#ifndef SshConnection_h
#define SshConnection_h

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
#include "SshConnectionAttributes.h"
#include "SshTunnelAttributes.h"
#include "WeakPointer.h"

#include <QBasicTimer>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTimerEvent>

namespace Ssh
{
    //_________________________________________________________
    class Connection: public QObject, public Counter
    {

        Q_OBJECT

        public:

        //* constructor
        Connection( QObject*, bool useThreads );

        //* destructor
        virtual ~Connection( void );

        //*@name accessors
        //@{

        //* connection attributes
        const ConnectionAttributes& attributes( void ) const
        { return attributes_; }

        //* connection state
        enum State
        {
            Uninitialized = 0,
            TunnelCreated = 1<<0,
            SessionCreated = 1<<1,
            Connected = 1<<2
        };

        Q_DECLARE_FLAGS( StateMask, State );

        StateMask state( void ) const
        { return state_; }

        //* true if connected
        bool isConnected( void ) const
        { return state_ & Connected; }

        //* true if disconnected
        bool isDisconnected( void ) const
        { return !(state_ & (TunnelCreated|SessionCreated|Connected)  ); }

        //* true if connecting
        bool isConnecting( void ) const
        { return !(isDisconnected() || isConnected() ); }

        //@}

        //*@name modifiers
        //@{

        using QObject::connect;

        //* set connection attributes
        void setAttributes( const ConnectionAttributes& attributes )
        { attributes_ = attributes; }

        //* tunnels
        bool createTunnels( void );

        //* connect
        bool connect( void );

        //* authenticate
        bool authenticate( bool forceRequestIdentity = false );

        //* command list
        enum SshCommand
        {
            None,
            Handshake,
            ConnectAgent,
            RequestIdentity,
            LoadAuthenticationMethods,
            ListIdentities,
            AuthenticateWithAgent,
            AuthenticateWithPassword
        };

        //* add command
        void addCommand( SshCommand );

        //* abort commands
        /* warning, if happens while SshInterface is in timer event, this might cause crash */
        void abortCommands( void );

        //* disconnect
        void disconnect( void );

        //@}

        //* true if ssh is supported
        static bool isSupported( void );

        Q_SIGNALS:

        //* login failed
        void loginFailed( void );

        //* message
        void message( const QString& );

        //* error message
        void error( const QString& );

        //* connected
        void connected( void );

        protected:

        //* timer event
        virtual void timerEvent( QTimerEvent* );

        protected Q_SLOTS:

        //* disconnect channels
        void _disconnectChannels( void );

        //* disconnect session
        void _disconnectSession( void );

        //* disconnect tunnel
        void _disconnectTunnels( void );

        //* error handling
        void _notifyError( QString );

        //* error handling
        void _notifyDebug( QString );

        //* new tcp connection (from QTcpServer)
        void _newConnection( void );

        //* new tcp connection (from threads)
        void _newConnection( int port, int socket );

        private:

        //* get message command
        QString _commandMessage( SshCommand ) const;

        //* abort all commands
        void _abortCommands( const QString& );

        //* use threads
        bool useThreads_ = false;

        //* ssh socket
        int sshSocket_ = -1;

        //* ssh session
        void* session_ = nullptr;

        //* agent
        void* agent_ = nullptr;

        //* identity
        void* identity_ = nullptr;

        //* mutex
        /* needed for concurent access to Ssh session, in thread mode */
        QMutex mutex_;

        //* connection attributes
        ConnectionAttributes attributes_;

        //* authentication methods
        QString authenticationMethods_;

        //* state
        StateMask state_ = Uninitialized;

        //* command list
        using CommandList = QList<SshCommand>;
        CommandList commands_;
        SshCommand lastCommand_ = None;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 20;

    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS( Ssh::Connection::StateMask );

#endif

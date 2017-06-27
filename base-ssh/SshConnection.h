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
#include <QObject>
#include <QString>
#include <QTimerEvent>
#include <QHostInfo>

namespace Ssh
{
    //_________________________________________________________
    class Connection: public QObject, private Base::Counter<Connection>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit Connection( QObject* );

        //* destructor
        ~Connection() override;

        //*@name accessors
        //@{

        //* connection attributes
        const ConnectionAttributes& attributes() const
        { return attributes_; }

        //* session
        void* session() const
        { return session_; }

        //* connection state
        enum State
        {
            Uninitialized = 0,
            TunnelCreated = 1<<0,
            SessionCreated = 1<<1,
            Connected = 1<<2
        };

        Q_DECLARE_FLAGS( StateMask, State );

        StateMask state() const
        { return state_; }

        //* true if connected
        bool isConnected() const
        { return state_ & Connected; }

        //* true if disconnected
        bool isDisconnected() const
        { return !(state_ & (TunnelCreated|SessionCreated|Connected)  ); }

        //* true if connecting
        bool isConnecting() const
        { return !(isDisconnected() || isConnected() ); }

        //* error string
        QString errorString() const
        { return error_; }

        //@}

        //*@name modifiers
        //@{

        using QObject::connect;

        //* set connection attributes
        void setAttributes( const ConnectionAttributes& attributes )
        { attributes_ = attributes; }

        //* tunnels
        bool createTunnels();

        //* connect
        bool connect();

        //* wait for connected
        /** warning, this method is blocking */
        bool waitForConnected( int msecs = 30000 );

        //* authenticate
        bool authenticate( bool forceRequestIdentity = false );

        //* command list
        enum Command
        {
            None,
            Connect,
            Handshake,
            ConnectAgent,
            RequestIdentity,
            LoadAuthenticationMethods,
            ListIdentities,
            AuthenticateWithAgent,
            AuthenticateWithPassword
        };

        //* add command
        void addCommand( Command );

        //* abort commands
        /* warning, if happens while SshInterface is in timer event, this might cause crash */
        void abortCommands();

        //* disconnect
        void disconnect();

        //@}

        //* true if ssh is supported
        static bool isSupported();

        Q_SIGNALS:

        //* message
        void message( const QString& );

        //* error message
        void error( const QString& );

        //* attributes updated
        void attributesChanged();

        //* connected
        void connected();

        //* login failed
        void loginFailed();

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        protected Q_SLOTS:

        //* save host info lookup
        void _saveHost( QHostInfo );

        //* disconnect channels
        void _disconnectChannels();

        //* disconnect session
        void _disconnectSession();

        //* disconnect tunnel
        void _disconnectTunnels();

        //* message handling
        void _notifyMessage( QString );

        //* error handling
        void _notifyError( QString );

        //* error handling
        void _notifyDebug( QString );

        //* new tcp connection (from QTcpServer)
        void _newConnection();

        private:

        //* get message command
        QString _commandMessage( Command ) const;

        //* process pending commands
        bool _processCommands();

        //* abort all commands
        void _abortCommands( const QString& );

        //* ssh socket
        int sshSocket_ = -1;

        //* ssh session
        void* session_ = nullptr;

        //* agent
        void* agent_ = nullptr;

        //* identity
        void* identity_ = nullptr;

        //* ssh host
        QHostInfo sshHost_;

        //* connection attributes
        ConnectionAttributes attributes_;

        //* authentication methods
        QString authenticationMethods_;

        //* state
        StateMask state_ = Uninitialized;

        //* error string
        QString error_;

        //* command list
        using CommandList = QList<Command>;
        CommandList commands_;
        Command lastCommand_ = None;

        //* timer
        QBasicTimer timer_;

        //* latency
        int latency_ = 10;

    };
}

Q_DECLARE_OPERATORS_FOR_FLAGS( Ssh::Connection::StateMask );

#endif

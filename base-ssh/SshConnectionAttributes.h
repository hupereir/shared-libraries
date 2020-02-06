#ifndef SshConnectionAttributes_h
#define SshConnectionAttributes_h

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

#include "base_ssh_export.h"
#include "SshTunnelAttributes.h"
#include "Counter.h"
#include "Functors.h"

#include <QString>
#include <QList>

namespace Ssh
{

    //_________________________________________________________
    class BASE_SSH_EXPORT ConnectionAttributes: private Base::Counter<ConnectionAttributes>
    {
        public:

        //* used to drag and drop in models
        static const QString MimeType;

        using List = QList<ConnectionAttributes>;
        using ListIterator = QListIterator<ConnectionAttributes>;

        //* constructor
        explicit ConnectionAttributes():
            Counter( QStringLiteral("Ssh::ConnectionAttributes") )
        {}

        virtual ~ConnectionAttributes() = default;

        //*@name accessors
        //@{

        //* name
        const QString& name() const
        { return name_; }

        //* host
        const QString& host() const
        { return host_; }

        //* user name
        const QString& user() const
        { return user_; }

        //* password
        const QString& password() const
        { return password_; }

        //* true if password is to be remembered
        bool rememberPassword() const
        { return rememberPassword_; }

        //* true if auto connect
        bool autoConnect() const
        { return autoConnect_; }

        //* port
        int port() const
        { return port_; }

        //* tunnels
        TunnelAttributes::Set tunnels() const
        { return tunnels_; }

        //* validity
        bool isValid() const;

        //@}

        //*@name modifiers
        //@{

        //* name
        void setName( const QString& name )
        { name_ = name; }

        //* host
        void setHost( const QString& host )
        { host_ = host; }

        //* user name
        void setUser( const QString& user )
        { user_ = user; }

        //* password
        void setPassword( const QString& password )
        { password_ = password; }

        //* remember password
        void setRememberPassword( bool value )
        { rememberPassword_ = value; }

        //* set auto connect
        void setAutoConnect( bool value )
        { autoConnect_ = value; }

        //* port
        void setPort( int port )
        { port_ = port; }

        //* set tunnels
        void setTunnels( const TunnelAttributes::List );

        //* add tunnel
        void addTunnel( const TunnelAttributes& );

        //@}

        //* used to find attributes with same names
        using SameNameFTor = Base::Functor::Unary<ConnectionAttributes, const QString&, &ConnectionAttributes::name>;

        private:

        //* connection name
        QString name_;

        //* host
        QString host_;

        //* user
        QString user_;

        //* password
        QString password_;

        //* remember password
        bool rememberPassword_ = false;

        //* auto connect
        bool autoConnect_ = false;

        //* port
        int port_ = 22;

        //* tunnels
        TunnelAttributes::Set tunnels_;

        //* equal to operator
        friend BASE_SSH_EXPORT bool operator == (const ConnectionAttributes&, const ConnectionAttributes&);

    };

    //* less than operator
    inline bool operator < (const ConnectionAttributes& first, const ConnectionAttributes& second)
    { return first.name() < second.name(); }

}

#endif

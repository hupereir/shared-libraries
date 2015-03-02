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

#include "SshTunnelAttributes.h"
#include "Counter.h"

#include <QString>
#include <QList>

namespace Ssh
{

    //_________________________________________________________
    class ConnectionAttributes: public Counter
    {
        public:

        using List = QList<ConnectionAttributes>;
        using ListIterator = QListIterator<ConnectionAttributes>;

        //* constructor
        ConnectionAttributes( void ):
            Counter( "Ssh::ConnectionAttributes" )
        {}

        //* destructor
        virtual ~ConnectionAttributes() = default;

        //* equal to operator
        bool operator == (const ConnectionAttributes& other) const
        {
            return
                name_ == other.name_ &&
                host_ == other.host_ &&
                userName_ == other.userName_ &&
                tunnels_ == other.tunnels_;
        }

        //* less than operator
        bool operator < ( const ConnectionAttributes& other ) const
        { return name_ < other.name_; }

        //*@name accessors
        //@{

        //* name
        const QString& name( void ) const
        { return name_; }

        //* host
        const QString& host( void ) const
        { return host_; }

        //* user name
        const QString& userName( void ) const
        { return userName_; }

        //* password
        const QString& password( void ) const
        { return password_; }

        //* true if password is to be remembered
        bool rememberPassword( void ) const
        { return rememberPassword_; }

        //* true if auto connect
        bool autoConnect( void ) const
        { return autoConnect_; }

        //* port
        int port( void ) const
        { return port_; }

        //* tunnels
        TunnelAttributes::Set tunnels( void ) const
        { return tunnels_; }

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
        void setUserName( const QString& userName )
        { userName_ = userName; }

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
        void setTunnels( const TunnelAttributes::List attributesList )
        {
            tunnels_.clear();
            foreach( const auto& attributes, attributesList )
            { addTunnel( attributes ); }
        }

        //* add tunnel
        void addTunnel( const TunnelAttributes& attributes )
        {
            auto iter = tunnels_.find( attributes );
            if( iter != tunnels_.end() ) tunnels_.erase( iter );
            tunnels_.insert( attributes );
        }

        //@}

        //* used to find attributes with same names
        class SameNameFTor
        {
            public:

            //* constructor
            SameNameFTor( const QString& name ):
                name_( name )
            {}

            //* predicate
            bool operator() (const ConnectionAttributes& other ) const
            { return other.name() == name_; }

            private:

            //* prediction
            const QString name_;

        };

        private:

        //* connection name
        QString name_;

        //* host
        QString host_;

        //* user
        QString userName_;

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

    };
}

#endif

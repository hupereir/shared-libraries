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

#include "SshConnectionAttributes.h"


namespace Ssh
{

    //_________________________________________________________________
    const QString ConnectionAttributes::MimeType( "ssh/connection-attributes" );

    //_____________________________________________________________
    bool ConnectionAttributes::operator == (const ConnectionAttributes& other) const
    {
        return
            name_ == other.name_ &&
            host_ == other.host_ &&
            userName_ == other.userName_ &&
            tunnels_ == other.tunnels_;
    }

    //_____________________________________________________________
    bool ConnectionAttributes::isValid( void ) const
    {
        if( host_.isEmpty() ) return false;
        for( auto tunnel:tunnels_ )
        { if( !tunnel.isValid() ) return false; }

        return true;
    }

    //_____________________________________________________________
    void ConnectionAttributes::setTunnels( const TunnelAttributes::List attributesList )
    {
        tunnels_.clear();
        for( auto attributes:attributesList )
        { addTunnel( attributes ); }
    }

    //_____________________________________________________________
    void ConnectionAttributes::addTunnel( const TunnelAttributes& attributes )
    {
        auto iter = tunnels_.find( attributes );
        if( iter != tunnels_.end() ) tunnels_.erase( iter );
        tunnels_.insert( attributes );
    }

}

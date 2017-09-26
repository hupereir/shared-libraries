#ifndef XcbConnection_h
#define XcbConnection_h

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

#include "XcbUtil.h"

#if HAVE_XCB
#include <xcb/xcb.h>
#endif

//* encapsulated xcb connection
class XcbConnection final: private Base::Counter<XcbConnection>
{
    public:

    //* constructor
    explicit XcbConnection():
        Counter( "XcbConnection" )
    {
        #if HAVE_XCB
        if( XcbUtil::isX11() )
        { connection_ = xcb_connect( nullptr, nullptr ); }
        #endif
    }

    //* destructor
    ~XcbConnection()
    {
        #if HAVE_XCB
        if( XcbUtil::isX11() )
        { xcb_disconnect( connection_ );  }
        #endif
    }

    #if HAVE_XCB

    //*@name accessors
    //@{

    //* accessor
    operator xcb_connection_t*() const { return connection_; }

    //* accessor
    xcb_connection_t* get() const { return connection_; }

    //@}

    private:

    //* the actual connection
    xcb_connection_t* connection_ = nullptr;

    #endif

};


#endif

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

#include "CompositeEngine.h"

#include "Debug.h"
#include "XcbUtil.h"

#if WITH_XCB
#include <xcb/xcb.h>
#endif

namespace Transparency
{

    //_______________________________________________________________
    CompositeEngine& CompositeEngine::get()
    {

        //! singleton
        static CompositeEngine singleton;
        return singleton;

    }

    //_______________________________________________________________
    CompositeEngine::CompositeEngine():
        available_( false ),
        initialized_( false )
    { Debug::Throw( QStringLiteral("CompositeEngine::CompositeEngine.\n") ); }

    //_______________________________________________________________
    void CompositeEngine::_initialize()
    {

        Debug::Throw( QStringLiteral("CompositeEngine::initialize\n") );
        if( !initialized_ )
        {
            initialized_ = true;
            available_ = _compositingAvailable();
        }
    }

    //_______________________________________________________________
    bool CompositeEngine::_compositingAvailable() const
    {
        Debug::Throw( QStringLiteral("CompositeEngine::_compositingAvailable\n") );

        #if defined(Q_OS_WIN)
        return true;
        #endif

        #if WITH_XCB

        if( !XcbUtil::isX11() ) return false;

        // connection
        xcb_connection_t* connection( XcbUtil::get().connection<xcb_connection_t>() );

        // atom
        const QString atomName( QStringLiteral( "_NET_WM_CM_S%1" ).arg( XcbUtil::get().defaultScreenNumber() ) );
        xcb_atom_t atom( *XcbUtil::get().atom<xcb_atom_t>( atomName ) );

        // selection owner
        xcb_get_selection_owner_cookie_t cookie( xcb_get_selection_owner( connection, atom ) );
        XcbUtil::ScopedPointer<xcb_get_selection_owner_reply_t> reply( xcb_get_selection_owner_reply( connection, cookie, nullptr ) );
        return reply && reply->owner;
        #endif

        // on all other systems, return false
        return false;

    }

}

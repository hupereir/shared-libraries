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

#include "KeyModifier.h"
#include "Debug.h"
#include "XcbUtil.h"


#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#if WITH_XCB
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/Xutil.h>
#endif

//______________________________________
KeyModifier::KeyModifier( Qt::Key key ):
key_( key )
{}

//_______________________________________
KeyModifier::State KeyModifier::state() const
{

    Debug::Throw( QStringLiteral("KeyModifier::state.\n") );

    #if defined(Q_OS_WIN)
    switch( key_ )
    {
        case Qt::Key_CapsLock: return ( GetKeyState(VK_CAPITAL) ) ? State::On:State::Off;
        case Qt::Key_NumLock: return ( GetKeyState(VK_NUMLOCK) ) ? State::On:State::Off;
        default: return State::Unknown;
    }

    #endif

    #if WITH_XCB

    if( !XcbUtil::isX11() ) return State::Unknown;

    // map Qt Key to X11
    int keySymbol(0);
    switch( key_ )
    {
        case Qt::Key_CapsLock:
        keySymbol = XK_Caps_Lock;
        break;

        case Qt::Key_NumLock:
        keySymbol = XK_Num_Lock;
        break;

        default:
        return State::Unknown;

    }

    // get matching key code
    auto connection( XcbUtil::get().connection<xcb_connection_t>() );
    auto symbols( xcb_key_symbols_alloc( connection ) );
    XcbUtil::ScopedPointer<xcb_keycode_t> keyCodes( xcb_key_symbols_get_keycode( symbols, keySymbol ) );

    // convert key codes to bit mask
    int keyMask( 0 );
    {
        auto&& cookie( xcb_get_modifier_mapping( connection ) );
        XcbUtil::ScopedPointer<xcb_get_modifier_mapping_reply_t> reply( xcb_get_modifier_mapping_reply( connection, cookie, nullptr ) );
        if( !reply ) return State::Unknown;

        // get modifiers
        auto&& modifiers( xcb_get_modifier_mapping_keycodes( reply.get() ) );
        const int count( xcb_get_modifier_mapping_keycodes_length( reply.get() ) );

        for( int i = 0; i<count; ++i )
        {
            if( modifiers[i] == keyCodes.get()[0] )
            {
                keyMask = 1 << i;
                break;
            }
        }

        keyCodes.reset();
        xcb_key_symbols_free(symbols);
    }

    // get key bits
    {
        auto&& cookie( xcb_query_pointer( connection, XcbUtil::get().appRootWindow() ) );
        XcbUtil::ScopedPointer<xcb_query_pointer_reply_t> reply( xcb_query_pointer_reply( connection, cookie, nullptr ) );
        if( reply ) return ( reply->mask & keyMask ) ? State::On : State::Off;
    }

    #endif

    return State::Unknown;

}

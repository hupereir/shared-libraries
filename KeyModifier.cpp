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

#if HAVE_XCB
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/Xutil.h>
#endif

//______________________________________
KeyModifier::KeyModifier( const Qt::Key& key ):
key_( key )
{}

//_______________________________________
KeyModifier::State KeyModifier::state( void ) const
{

    Debug::Throw( "KeyModifier::state.\n" );

    #if defined(Q_OS_WIN)
    if( key_ == Qt::Key_CapsLock ) return ( GetKeyState(VK_CAPITAL) ) ? On:Off;
    else if( key_ == Qt::Key_NumLock ) return ( GetKeyState(VK_NUMLOCK) ) ? On:Off;
    else return Unknown;
    #endif

    #if HAVE_XCB
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
        return Unknown;

    }

    // get matching key code
    xcb_connection_t* connection( XcbUtil::get().connection<xcb_connection_t>() );
    xcb_key_symbols_t *symbols( xcb_key_symbols_alloc( connection ) );
    xcb_keycode_t* keyCodes( xcb_key_symbols_get_keycode( symbols, keySymbol ) );

    // convert key codes to bit mask
    int keyMask( 0 );
    {
        xcb_get_modifier_mapping_cookie_t cookie( xcb_get_modifier_mapping( connection ) );
        XcbUtil::ScopedPointer<xcb_get_modifier_mapping_reply_t> reply( xcb_get_modifier_mapping_reply( connection, cookie, 0x0 ) );
        if( !reply ) return Unknown;

        // get modifiers
        xcb_keycode_t *modifiers( xcb_get_modifier_mapping_keycodes( reply.data() ) );
        const int count( xcb_get_modifier_mapping_keycodes_length( reply.data() ) );

        for( int i = 0; i<count; ++i )
        {
            if( modifiers[i] == keyCodes[0] )
            { keyMask = 1 << i; }
        }

        free( keyCodes );
        xcb_key_symbols_free(symbols);
    }

    // get key bits
    {
        xcb_query_pointer_cookie_t cookie( xcb_query_pointer( connection, XcbUtil::get().appRootWindow() ) );
        XcbUtil::ScopedPointer<xcb_query_pointer_reply_t> reply( xcb_query_pointer_reply( connection, cookie, 0x0 ) );
        if( reply ) return ( reply.data()->mask & keyMask ) ? On:Off;
    }

    #endif

    return Unknown;

}

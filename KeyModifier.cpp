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

#include "KeyModifier.h"

#include "Debug.h"
#include "X11Util.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
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
    else return UNKNOWN;
    #endif

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    // map Qt Key to X11
    int key_symbol(0);
    switch( key_ )
    {
        case Qt::Key_CapsLock:
        key_symbol = XK_Caps_Lock;
        break;

        case Qt::Key_NumLock:
        key_symbol = XK_Num_Lock;
        break;

        default:
        return Unknown;

    }

    // get matching key code
    Display* display = reinterpret_cast<Display*>(X11Util::get().display());
    KeyCode key_code = XKeysymToKeycode( display, key_symbol );

    // convert key code to bit mask
    XModifierKeymap* modifiers = XGetModifierMapping(display);
    int key_mask = 0;
    for( int i = 0; i<8; i++ )
    {
        if( modifiers->modifiermap[modifiers->max_keypermod * i] == key_code)
        { key_mask = 1 << i; }
    }

    // get key bits
    unsigned int key_bits;
    Window window_1, window_2;
    int i3, i4, i5, i6;
    XQueryPointer(
        display, DefaultRootWindow(display), &window_1, &window_2,
        &i3, &i4, &i5, &i6, &key_bits
        );

    XFreeModifiermap( modifiers );

    // compare bits to maks
    return ( key_bits & key_mask ) ? On:Off;

    #else

    return Unknown;

    #endif
}

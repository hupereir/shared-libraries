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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file    KeyModifier.cpp
  \brief   key state of given modifier
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "Debug.h"
#include "KeyModifier.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

// this is a hack to allow compilation at BNL
// not sure whether this is armless or not
#define XK_MISCELLANY
#include <X11/keysymdef.h>

#endif

//______________________________________
KeyModifier::KeyModifier( const Qt::Key& key ):
  key_( key )
  {}

//_______________________________________
KeyModifier::State KeyModifier::state( void ) const
{

  Debug::Throw( "KeyModifier::state.\n" );

  #ifdef Q_WS_WIN
  if( key_ == Qt::Key_CapsLock ) return ( GetKeyState(VK_CAPITAL) ) ? ON:OFF;
  else if( key_ == Qt::Key_NumLock ) return ( GetKeyState(VK_NUMLOCK) ) ? ON:OFF;
  else return UNKNOWN;
  #endif

  #ifdef Q_WS_X11
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
    return UNKNOWN;

  }

  // get matching key code
  Display* display( QX11Info::display() );
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
  return ( key_bits & key_mask ) ? ON:OFF;

  #else

  return UNKNOWN;

  #endif
}

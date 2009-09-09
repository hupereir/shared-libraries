
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
  \file    X11Util.h
  \brief   some X11 specific utilities
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "Debug.h"
#include "X11Util.h"

#include <QDesktopWidget>

using namespace std;

//________________________________________________________________________
X11Util& X11Util::get( void )
{
  static X11Util singleton;
  return singleton;
}

//________________________________________________________________________
X11Util::X11Util( void )
{
  Debug::Throw( "X11Util::X11Util" );
  _initializeAtomNames();
}

//________________________________________________________________________
bool X11Util::isSupported( const Atoms& atom )
{

  #ifdef Q_WS_X11

  Debug::Throw() << "X11Util::isSupported - " << atom_names_[atom] << endl;

  SupportedAtomMap::const_iterator iter( _supportedAtoms().find( atom ) );
  if( iter != _supportedAtoms().end() )
  {
    Debug::Throw() << "X11Util::isSupported - " << atom_names_[atom] << (iter->second ? " true ":" false ") << endl;
    return iter->second;
  }

  Display* display( QX11Info::display() );
  Atom net_supported( findAtom( _NET_SUPPORTED ) );
  Atom searched( findAtom( atom ) );

  Atom actual;
  int format;
  unsigned char *data;
  unsigned long offset = 0;

  while( 1 )
  {
    unsigned long n, left;
    XGetWindowProperty( display, QX11Info::appRootWindow(),
      net_supported, offset, 1L,
      false, XA_ATOM, &actual,
      &format, &n, &left,
      (unsigned char **) &data);

    if( data == None ) break;

    // try cast data to atom
    Atom found( *(Atom*)data );

    if( found == searched )
    {
      supported_atoms_[atom] = true;
      Debug::Throw() << "X11Util::isSupported - " << atom_names_[atom] << " true " << endl;
      return true;
    }

    if( !left ) break;
    else offset ++;

  }

  supported_atoms_[atom] = false;
  Debug::Throw() << "X11Util::isSupported - " << atom_names_[atom] << " false " << endl;

  #endif

  return false;

}

//________________________________________________________________________
bool X11Util::hasProperty( const QWidget& widget, const Atoms& atom )
{

  Debug::Throw( "X11Util::hasProperty.\n" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom searched( findAtom( atom ) );

  Atom actual;
  int format;
  unsigned char *data;

  // try retrieve property
  unsigned long offset = 0;
  while( 1 )
  {
    unsigned long n, left;
    XGetWindowProperty(
      display, widget.winId(), net_wm_state,
      offset, 1L, false,
      XA_ATOM, &actual,
      &format, &n, &left,
      (unsigned char **) &data);

    // finish if no data is found
    if( data == None ) break;

    // try cast data to atom and compare
    Atom found( *(Atom*)data );
    if( found == searched ) return true;

    // move forward if more data remains
    if( !left ) break;
    else offset ++;

  }

  // no match found
  return false;

  #else
  return false;
  #endif

}

//_______________________________________________________
unsigned long X11Util::cardinal( const QWidget& widget, const Atoms& atom )
{

  Debug::Throw( "X11Util::cardinal" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom searched( findAtom(atom) );
  Atom actual;
  int format;
  unsigned char *data;

  unsigned long n, left;
  XGetWindowProperty(
    display, widget.winId(), searched,
    0, 1L, false,
    XA_CARDINAL, &actual,
    &format, &n, &left,
    (unsigned char **) &data);

  // finish if no data is found
  if( data == None ) return 0;

  // try cast data to atom and compare
  unsigned long found( *(int*)data );
  return found;

  #else
  return 0;
  #endif

}

//________________________________________________________________________
bool X11Util::changeProperty( const QWidget& widget, const Atoms& atom, const int& nelements )
{

  Debug::Throw( "X11Util::changeProperty.\n" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom searched( findAtom( atom ) );

  XChangeProperty (display, widget.winId(), net_wm_state, XA_ATOM, 32, PropModeAppend, (unsigned char *)&searched, nelements );
  return true;
  #endif

  return false;
}


//________________________________________________________________________
bool X11Util::changeCardinal( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

  Debug::Throw( "X11Util::changeProperty.\n" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom searched( findAtom( atom ) );

  XChangeProperty (display, widget.winId(), searched, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&value, 1 );
  return true;
  #endif

  return false;
}

//________________________________________________________________________
bool X11Util::removeProperty( const QWidget& widget, const Atoms& atom )
{
  Debug::Throw( "X11Util::removeProperty.\n" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom searched( findAtom( atom ) );

  Atom actual;
  int format;
  unsigned char *data;

  // try retrieve property
  unsigned long offset = 0;
  std::list<Atom> atoms;
  while( 1 )
  {
    unsigned long n, left;
    XGetWindowProperty(
      display, widget.winId(), net_wm_state,
      offset, 1L, false,
      XA_ATOM, &actual,
      &format, &n, &left,
      (unsigned char **) &data);

    if( data == None ) break;

    // try cast data to atom
    Atom found( *(Atom*)data );
    if( found != searched ) atoms.push_back( found );
    if( !left ) break;
    else offset ++;
  }

  // delete property
  XDeleteProperty( display, widget.winId(), net_wm_state );

  // re-add atoms that are not the one to be deleted
  for( std::list<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); iter++ )
  {XChangeProperty( display, widget.winId(), net_wm_state, XA_ATOM, 32, PropModeAppend, (unsigned char *)&*iter, 1); }
  return true;
  #endif

  return false;
}

//________________________________________________________________________
bool X11Util::moveResizeWidget(
  const QWidget& widget,
  const QPoint& position,
  const X11Util::Direction& direction,
  Qt::MouseButton button )
{

  Debug::Throw() << "X11Util::moveResizeWidget - (" << position.x() << "," << position.y() << ")" << endl;

  if( !widget.isWindow() ) return false;

  #ifdef Q_WS_X11

  // check
  if( !isSupported( _NET_WM_MOVERESIZE ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_moveresize( findAtom( _NET_WM_MOVERESIZE ) );

  XEvent event;
  event.xclient.type = ClientMessage;
  event.xclient.message_type = net_wm_moveresize;
  event.xclient.display = display;
  event.xclient.window = widget.winId();
  event.xclient.format = 32;
  event.xclient.data.l[0] = position.x();
  event.xclient.data.l[1] = position.y();
  event.xclient.data.l[2] = direction;
  event.xclient.data.l[3] = button;
  event.xclient.data.l[4] = 0;
  XUngrabPointer( display, QX11Info::appTime() );
  XSendEvent(display,
    QX11Info::appRootWindow( widget.x11Info().screen()), false,
    SubstructureRedirectMask | SubstructureNotifyMask, &event);
  return true;

  #else
  return false;
  #endif
}



//________________________________________________________________________
void X11Util::_initializeAtomNames( void )
{

  Debug::Throw( "X11Util::_initializeAtomNames.\n" );

  atom_names_[_NET_SUPPORTED] = "_NET_SUPPORTED";
  atom_names_[_NET_WM_DESKTOP] = "_NET_WM_DESKTOP";
  atom_names_[_NET_WM_STATE] = "_NET_WM_STATE";
  atom_names_[_NET_WM_STATE_STICKY] = "_NET_WM_STATE_STICKY";
  atom_names_[_NET_WM_STATE_STAYS_ON_TOP] = "_NET_WM_STATE_STAYS_ON_TOP";
  atom_names_[_NET_WM_STATE_ABOVE] = "_NET_WM_STATE_ABOVE";
  atom_names_[_NET_WM_STATE_SKIP_TASKBAR] = "_NET_WM_STATE_SKIP_TASKBAR";
  atom_names_[_NET_WM_MOVERESIZE] = "_NET_WM_MOVERESIZE";
  atom_names_[_NET_WM_CM] = "_NET_WM_CM";

  return;
}

#ifdef Q_WS_X11

//________________________________________________________________________
Atom X11Util::findAtom( const Atoms& atom )
{

  Debug::Throw( "X11Util::findAtom.\n" );

  // find atom in map
  AtomMap::iterator iter( _atoms().find( atom ) );
  if( iter != _atoms().end() ) return iter->second;

  // create atom if not found
  Display* display( QX11Info::display() );
  Atom out( XInternAtom(display, qPrintable( atom_names_[atom] ), false ) );
  atoms_[atom] = out;
  return out;

}

#endif

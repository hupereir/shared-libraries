
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

//_______________________
int debug_level = 1;

#ifdef Q_WS_X11
const unsigned long netwm_sendevent_mask = (
  SubstructureRedirectMask|
  SubstructureNotifyMask);
#endif

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

  Debug::Throw( debug_level ) << "X11Util::isSupported - " << atom_names_[atom] << endl;

  SupportedAtomMap::const_iterator iter( _supportedAtoms().find( atom ) );
  if( iter != _supportedAtoms().end() )
  {
    Debug::Throw() << "X11Util::isSupported - " << atom_names_[atom] << (iter->second ? " true ":" false ") << endl;
    return iter->second;
  }

  Display* display( QX11Info::display() );
  Atom net_supported( findAtom( _NET_SUPPORTED ) );
  Atom searched( findAtom( atom ) );

  Atom type;
  int format;
  unsigned char *data;
  unsigned long count;
  unsigned long unused;

  // get window property
  if( XGetWindowProperty(
    display, QX11Info::appRootWindow(),
    net_supported, 0l, 2048l,
    false, XA_ATOM, &type,
    &format, &count, &unused, (unsigned char **) &data) != Success )
  {
    Debug::Throw(debug_level) << "X11Util::isSupported - XGetProperty failed" << endl;
    return false;
  }

  if( type != XA_ATOM || format != 32 || count <= 0 || data == None )
  {
    Debug::Throw(debug_level) << "X11Util::isSupported - incorrect output for XGetProperty" << endl;
    return false;
  }

  Debug::Throw(debug_level) << "X11Util::isSupported - count: " << count << endl;
  long *states = (long *) data;
  bool found = false;
  static bool first( true );
  for( unsigned long i = 0; i<count; i++ )
  {

    if( first && Debug::level() >= debug_level )
    {
      char* data = XGetAtomName(display, (Atom) states[i]);
      Debug::Throw(debug_level) << "X11Util::isSupported - supported: " << data << endl;
      if ( data ) XFree( data );
    }

    if( searched == states[i] ) {

      found = true;
      if( !( first && Debug::level() < debug_level ) ) break;

    }

  }

  if( first ) first = false;
  supported_atoms_[atom] = found;
  return found;

  #endif

  return false;

}

//________________________________________________________________________
bool X11Util::hasProperty( const QWidget& widget, const Atoms& atom )
{

  Debug::Throw(debug_level) << "X11Util::hasProperty - " << atom_names_[atom] << endl;

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom searched( findAtom( atom ) );

  Atom type;
  int format;
  unsigned char *data;
  unsigned long count;
  unsigned long unused;

  // get window property
  if( XGetWindowProperty(
     display, widget.winId(), net_wm_state,
     0L, 2048L, false,
     XA_ATOM, &type,
     &format, &count, &unused,
     (unsigned char **) &data) != Success )
  {
    Debug::Throw(debug_level) << "X11Util::hasProperty - XGetProperty failed" << endl;
    return false;
  }

  Debug::Throw(debug_level) << "X11Util::hasProperty -"
    << " count: " << count
    << " unused: " << unused
    << endl;

  if( format != 32 )
  {
    Debug::Throw(debug_level) << "X11Util::hasProperty - wrong format: " << format << endl;
    return false;
  }

  if( count <= 0 )
  {
    Debug::Throw(debug_level) << "X11Util::hasProperty - wrong count: " << count << endl;
    return false;
  }

  if( data == None )
  {
    Debug::Throw(debug_level) << "X11Util::hasProperty - wrong data." << endl;
    return false;
  }

  long *states = (long *) data;
  for( unsigned long i = 0; i<count; i++ )
  {

    if( Debug::level() >= debug_level )
    {
      char* data = XGetAtomName(display, (Atom) states[i]);
      Debug::Throw(debug_level) << "X11Util::hasProperty - found: " << data << endl;
      if ( data ) XFree( data );
    }

    if( searched == (Atom) states[i] ) return true;

  }

  // no match found
  return false;

  #else
  return false;
  #endif

}

//_______________________________________________________
unsigned long X11Util::cardinal( const WId& wid, const Atoms& atom )
{

  Debug::Throw( "X11Util::cardinal" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom searched( findAtom(atom) );
  Atom type;
  int format;
  unsigned char *data;

  unsigned long n, left;
  XGetWindowProperty(
    display, wid, searched,
    0, 1L, false,
    XA_CARDINAL, &type,
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
bool X11Util::_changeProperty( const QWidget& widget, const Atoms& atom, bool state )
{

  Debug::Throw(debug_level) << "X11Util::_changeProperty - atom: " << atom_names_[atom] << " state: " << state << endl;

  #ifdef Q_WS_X11
  //printWindowState( widget );

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom searched( findAtom( atom ) );

  Atom type;
  int format;
  unsigned char *data;
  unsigned long count;
  unsigned long unused;
  std::list<Atom> atoms;

  // get window property
  if( XGetWindowProperty(
     display, widget.winId(), net_wm_state,
     0L, 2048L, false,
     XA_ATOM, &type,
     &format, &count, &unused,
     (unsigned char **) &data) != Success )
  {
    Debug::Throw(debug_level) << "X11Util::_changeProperty - XGetProperty failed" << endl;
    return false;
  }

  Debug::Throw(debug_level) << "X11Util::_changeProperty -"
    << " count: " << count
    << " unused: " << unused
    << endl;

  bool found( false );
  long *states = (long *) data;
  for( unsigned long i = 0; i<count; i++ )
  {

    if( Debug::level() >= debug_level )
    {
      char* data = XGetAtomName(display, (Atom) states[i]);
      Debug::Throw(debug_level) << "X11Util::_changeProperty - found: " << data << endl;
      if ( data ) XFree( data );
    }

    if( searched == (Atom) states[i] )
    {

      found = true;
      if( state ) atoms.push_back( states[i] );

    } else atoms.push_back( states[i] );

  }

  if( !( state || found ) ) return true;

  if( state && !found ) atoms.push_back( searched );

  {
    long data[50];
    int count = 0;

    atoms.push_back( searched );
    for( std::list<Atom>::iterator iter = atoms.begin(); iter != atoms.end(); iter++ )
    { data[count++] = *iter; }

    XChangeProperty( display, widget.winId(), net_wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)data, count);
  }

  //printWindowState( widget );
  return true;
  #endif

  return false;
}

//________________________________________________________________________
bool X11Util::_requestPropertyChange( const QWidget& widget, const Atoms& atom, bool value )
{

  Debug::Throw(debug_level) << "X11Util::_requestPropertyChange - atom: " << atom_names_[atom] << " state: " << value << endl;

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );
  Atom requested( findAtom( atom ) );

	XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = net_wm_state;
	e.xclient.display = display;
	e.xclient.window = widget.winId();
	e.xclient.format = 32;
  e.xclient.data.l[0] = (value) ? 1 : 0;
  e.xclient.data.l[1] = requested;
  e.xclient.data.l[2] = 0l;
  e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;

  XSendEvent( display, QX11Info::appRootWindow(), False, netwm_sendevent_mask, &e);

  return true;

  #endif

  return false;
}

//________________________________________________________________________
bool X11Util::_changeCardinal( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

  Debug::Throw( "X11Util::_changeCardinal.\n" );

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
bool X11Util::_requestCardinalChange( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

  Debug::Throw( "X11Util::_requestChangeCardinal.\n" );

  #ifdef Q_WS_X11

  // make sure atom is supported
  if( !isSupported( atom ) ) return false;

  Display* display( QX11Info::display() );
  Atom requested( findAtom( atom ) );

  XEvent e;
	e.xclient.type = ClientMessage;
	e.xclient.message_type = requested;
	e.xclient.display = display;
	e.xclient.window = widget.winId();
	e.xclient.format = 32;
	e.xclient.data.l[0] = value;
	e.xclient.data.l[1] = 0l;
	e.xclient.data.l[2] = 0l;
	e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;

  XSendEvent(display, QX11Info::appRootWindow(), false, netwm_sendevent_mask, &e);

  return true;
  #endif

  return false;
}

//________________________________________________________________________
void X11Util::_initializeAtomNames( void )
{

  Debug::Throw( "X11Util::_initializeAtomNames.\n" );

  atom_names_[_NET_SUPPORTED] = "_NET_SUPPORTED";
  atom_names_[_NET_CURRENT_DESKTOP] = "_NET_CURRENT_DESKTOP";
  atom_names_[_NET_WM_DESKTOP] = "_NET_WM_DESKTOP";
  atom_names_[_NET_WM_STATE] = "_NET_WM_STATE";
  atom_names_[_NET_WM_STATE_STICKY] = "_NET_WM_STATE_STICKY";
  atom_names_[_NET_WM_STATE_STAYS_ON_TOP] = "_NET_WM_STATE_STAYS_ON_TOP";
  atom_names_[_NET_WM_STATE_ABOVE] = "_NET_WM_STATE_ABOVE";
  atom_names_[_NET_WM_STATE_SKIP_TASKBAR] = "_NET_WM_STATE_SKIP_TASKBAR";
  atom_names_[_NET_WM_STATE_SKIP_PAGER] = "_NET_WM_STATE_SKIP_PAGER";
  atom_names_[_NET_WM_MOVERESIZE] = "_NET_WM_MOVERESIZE";
  atom_names_[_NET_WM_CM] = "_NET_WM_CM";

  return;
}

//________________________________________________________
void X11Util::printWindowState( const QWidget& widget )
{

  #ifdef Q_WS_X11
  Display* display( QX11Info::display() );
  Atom net_wm_state( findAtom(_NET_WM_STATE) );

  Atom type;
  int format;
  unsigned char *data;
  unsigned long count;
  unsigned long unused;
  std::list<Atom> atoms;

  // get window property
  if( XGetWindowProperty(
     display, widget.winId(), net_wm_state,
     0L, 2048L, false,
     XA_ATOM, &type,
     &format, &count, &unused,
     (unsigned char **) &data) != Success )
  {
    Debug::Throw(debug_level) << "X11Util::printWindowState - XGetProperty failed" << endl;
    return;
  }

  Debug::Throw(debug_level) << "X11Util::printWindowState -"
    << " count: " << count
    << " unused: " << unused
    << endl;

  long *states = (long *) data;
  Debug::Throw(0) << "X11Util::printWindowState - ";
  for( unsigned long i = 0; i<count; i++ )
  {

    char* data = XGetAtomName(display, (Atom) states[i]);
    Debug::Throw(0) << " " << data;
    if ( data ) XFree( data );

  }

  Debug::Throw(0) << endl;
  #endif

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

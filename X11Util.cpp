
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

#include "Debug.h"
#include "X11Util.h"

#include <QDesktopWidget>

//_______________________
int debug_level = 1;

#if defined(Q_WS_X11)
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

    #if defined(Q_WS_X11)

    Debug::Throw( debug_level ) << "X11Util::isSupported - " << atomNames_[atom] << endl;

    SupportedAtomMap::const_iterator iter( _supportedAtoms().find( atom ) );
    if( iter != _supportedAtoms().end() )
    {
        Debug::Throw() << "X11Util::isSupported - " << atomNames_[atom] << (iter.value() ? " true ":" false ") << endl;
        return iter.value();
    }

    Display* display( QX11Info::display() );
    Atom net_supported( findAtom( _NET_SUPPORTED ) );
    Atom searched( findAtom( atom ) );

    Atom type;
    int format;
    unsigned char *data;
    unsigned long count;
    unsigned long after;

    int length = 32768;
    while( true )
    {

        // get window property
        bool success( XGetWindowProperty(
            display, QX11Info::appRootWindow(),
            net_supported, 0l, length,
            false, XA_ATOM, &type,
            &format, &count, &after, &data) == Success );

        if( !success )
        {
            Debug::Throw(debug_level) << "X11Util::isSupported - XGetProperty failed" << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    if( type != XA_ATOM || format != 32 || count <= 0 || data == None )
    {
        Debug::Throw(debug_level) << "X11Util::isSupported - incorrect output for XGetProperty" << endl;
        if( data ) XFree( data );
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

        if( searched == (Atom) states[i] )
        {

            found = true;
            if( !( first && Debug::level() < debug_level ) ) break;

        }

    }

    if( first ) first = false;
    supportedAtoms_[atom] = found;
    if ( data ) XFree( data );

    return found;

    #endif

    return false;

}

//________________________________________________________________________
bool X11Util::hasProperty( const QWidget& widget, const Atoms& atom )
{

    Debug::Throw(debug_level) << "X11Util::hasProperty - " << atomNames_[atom] << endl;

    #if defined(Q_WS_X11)

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Display* display( QX11Info::display() );
    Atom net_wm_state( findAtom(_NET_WM_STATE) );
    Atom searched( findAtom( atom ) );

    Atom type;
    int format;
    unsigned char *data;
    unsigned long count;
    unsigned long after;

    int length = 32768;
    while( true )
    {

        // get window property
        bool success( XGetWindowProperty(
            display, widget.winId(), net_wm_state,
            0L, length, false,
            XA_ATOM, &type,
            &format, &count, &after,
            &data) == Success );

        if( !success )
        {
            Debug::Throw(debug_level) << "X11Util::hasProperty - XGetProperty failed" << endl;
            return false;
        }

        if( format != 32 )
        {
            Debug::Throw(debug_level) << "X11Util::hasProperty - wrong format: " << format << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    Debug::Throw(debug_level) << "X11Util::hasProperty -"
        << " count: " << count
        << endl;

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

        if( searched == (Atom) states[i] )
        {
            XFree( data );
            return true;
        }

    }

    // no match found
    XFree( data );
    return false;

    #else
    return false;
    #endif

}

//_______________________________________________________
unsigned long X11Util::cardinal( const WId& wid, const Atoms& atom )
{

    Debug::Throw( "X11Util::cardinal" );

    #if defined(Q_WS_X11)

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
        &data);

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

    #if defined(Q_WS_X11)

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
        QX11Info::appRootWindow( widget.x11Info().screen()), false, netwm_sendevent_mask, &event);
    return true;

    #else
    return false;
    #endif
}

//________________________________________________________________________
bool X11Util::changeProperty( const QWidget& widget, const Atoms& atom, const unsigned char* data, int size )
{
    #if defined(Q_WS_X11)
    Display* display( QX11Info::display() );
    Atom searched( findAtom( atom ) );
    XChangeProperty (display, widget.winId(), searched, XA_CARDINAL, 32, PropModeReplace, data, size );

    return true;
    #endif

    return false;

}

//________________________________________________________________________
bool X11Util::_changeProperty( const QWidget& widget, const Atoms& atom, bool state )
{

    Debug::Throw(debug_level) << "X11Util::_changeProperty - atom: " << atomNames_[atom] << " state: " << state << endl;

    #if defined(Q_WS_X11)

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Display* display( QX11Info::display() );
    Atom net_wm_state( findAtom(_NET_WM_STATE) );
    Atom searched( findAtom( atom ) );

    Atom type;
    int format;
    unsigned char *data;
    unsigned long count;
    unsigned long after;
    QVector<Atom> atoms;

    int length = 32768;
    while( true )
    {

        // get window property
        bool success( XGetWindowProperty(
            display, widget.winId(), net_wm_state,
            0L, length, false,
            XA_ATOM, &type,
            &format, &count, &after,
            &data ) == Success );

        if( !success )
        {
            Debug::Throw(debug_level) << "X11Util::_changeProperty - XGetProperty failed" << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    Debug::Throw(debug_level) << "X11Util::_changeProperty -"
        << " count: " << count
        << endl;

    bool found( false );
    Atom *states = reinterpret_cast<Atom*>(data);
    for( unsigned long i = 0; i<count; i++ )
    {

        if( Debug::level() >= debug_level )
        {
            char* data = XGetAtomName(display, states[i]);
            Debug::Throw(debug_level) << "X11Util::_changeProperty - found: " << data << endl;
            if ( data ) XFree( data );
        }

        if( searched == states[i] )
        {

            found = true;
            if( state ) atoms << states[i];

        } else atoms << states[i];

    }

    if( state == found )
    {
        if ( data ) XFree( data );
        return true;
    }

    if( state && !found ) atoms << searched;
    XChangeProperty( display, widget.winId(), net_wm_state, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(atoms.constData()), atoms.size() );

    if ( data ) XFree( data );
    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool X11Util::_requestPropertyChange( const QWidget& widget, const Atoms& atom, bool value )
{

    Debug::Throw(debug_level) << "X11Util::_requestPropertyChange - atom: " << atomNames_[atom] << " state: " << value << endl;

    #if defined(Q_WS_X11)

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

    #if defined(Q_WS_X11)

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Display* display( QX11Info::display() );
    Atom searched( findAtom( atom ) );

    XChangeProperty (display, widget.winId(), searched, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char *>(&value), 1 );
    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool X11Util::_requestCardinalChange( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

    Debug::Throw( "X11Util::_requestChangeCardinal.\n" );

    #if defined(Q_WS_X11)

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

    atomNames_[_NET_SUPPORTED] = "_NET_SUPPORTED";
    atomNames_[_NET_CURRENT_DESKTOP] = "_NET_CURRENT_DESKTOP";
    atomNames_[_NET_WM_DESKTOP] = "_NET_WM_DESKTOP";
    atomNames_[_NET_WM_STATE] = "_NET_WM_STATE";
    atomNames_[_NET_WM_STATE_STICKY] = "_NET_WM_STATE_STICKY";
    atomNames_[_NET_WM_STATE_STAYS_ON_TOP] = "_NET_WM_STATE_STAYS_ON_TOP";
    atomNames_[_NET_WM_STATE_ABOVE] = "_NET_WM_STATE_ABOVE";
    atomNames_[_NET_WM_STATE_SKIP_TASKBAR] = "_NET_WM_STATE_SKIP_TASKBAR";
    atomNames_[_NET_WM_STATE_SKIP_PAGER] = "_NET_WM_STATE_SKIP_PAGER";
    atomNames_[_NET_WM_MOVERESIZE] = "_NET_WM_MOVERESIZE";
    atomNames_[_NET_WM_CM] = "_NET_WM_CM";
    atomNames_[_KDE_NET_WM_BLUR_BEHIND_REGION] = "_KDE_NET_WM_BLUR_BEHIND_REGION";

    return;
}

//________________________________________________________
void X11Util::printWindowState( const QWidget& widget )
{

    #if defined(Q_WS_X11)
    Display* display( QX11Info::display() );
    Atom net_wm_state( findAtom(_NET_WM_STATE) );

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
        &data) != Success )
    {
        Debug::Throw(debug_level) << "X11Util::printWindowState - XGetProperty failed" << endl;
        return;
    }

    Debug::Throw(debug_level) << "X11Util::printWindowState -"
        << " count: " << count
        << " unused: " << unused
        << endl;

    Atom* states = reinterpret_cast<Atom*>(data);
    Debug::Throw(0) << "X11Util::printWindowState - ";
    for( unsigned long i = 0; i<count; i++ )
    {

        char* data = XGetAtomName(display, states[i]);
        Debug::Throw(0) << " " << data;
        if ( data ) XFree( data );

    }

    Debug::Throw(0) << endl;
    #endif

}

#if defined(Q_WS_X11)

//________________________________________________________________________
Atom X11Util::findAtom( const Atoms& atom )
{

    Debug::Throw( "X11Util::findAtom.\n" );

    // find atom in map
    AtomMap::iterator iter( _atoms().find( atom ) );
    if( iter != _atoms().end() ) return iter.value();

    // create atom if not found
    Display* display( QX11Info::display() );
    Atom out( XInternAtom(display, qPrintable( atomNames_[atom] ), false ) );
    atoms_[atom] = out;
    return out;

}

#endif

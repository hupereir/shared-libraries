
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

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
// #include <5.0.0/QtGui/qpa/qplatformnativeinterface.h>
#include <5.0.1/QtGui/qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

//_______________________
int debugLevel = 1;

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
const unsigned long netWMSendEventMask = (
    SubstructureRedirectMask|
    SubstructureNotifyMask);
#endif

//________________________________________________________________________
class X11Util::Private
{

    public:

    //! constructor
    Private( void );

    X11Util::AtomNameMap atomNames_;

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    //!display
    Display* display( void );

    //! find atom
    Atom findAtom( const Atoms& atom );

    //! display
    Display* display_;

    //! atom map
    typedef QHash<Atoms, Atom> AtomMap;

    //! atoms
    AtomMap atoms_;

    #endif

};

//________________________________________________________________________
X11Util::Private::Private( void )
{

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    display_ = 0;
    #endif

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

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
//_______________________________________________________
Display* X11Util::Private::display( void )
{
    if( !display_ )
    {

        #if QT_VERSION >= 0x050000
        QPlatformNativeInterface *native = qApp->platformNativeInterface();
        void *display = native->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen());
        display_ = reinterpret_cast<Display*>(display);
        #else
        if( !display_ ) display_ = QX11Info::display();
        #endif

    }
    return display_;
}

//________________________________________________________________________
Atom X11Util::Private::findAtom( const Atoms& atom )
{

    Debug::Throw( "X11Util::Private::findAtom.\n" );

    // find atom in map
    Private::AtomMap::iterator iter( atoms_.find( atom ) );
    if( iter != atoms_.end() ) return iter.value();

    // create atom if not found
    Atom out( XInternAtom( display(), qPrintable( atomNames_[atom] ), false ) );
    atoms_[atom] = out;
    return out;

}

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
    d = new Private();
}

//________________________________________________________________________
X11Util::~X11Util( void )
{ delete d; }

//________________________________________________________________________
Qt::HANDLE X11Util::display( void ) const
{
    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    return Qt::HANDLE( d->display() );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE X11Util::findAtom( const X11Util::Atoms& atom ) const
{
    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    return Qt::HANDLE( d->findAtom( atom ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
bool X11Util::isSupported( const Atoms& atom )
{

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    Debug::Throw( debugLevel ) << "X11Util::isSupported - " << d->atomNames_[atom] << endl;

    SupportedAtomMap::const_iterator iter( supportedAtoms_.find( atom ) );
    if( iter != supportedAtoms_.end() )
    {
        Debug::Throw() << "X11Util::isSupported - " << d->atomNames_[atom] << (iter.value() ? " true ":" false ") << endl;
        return iter.value();
    }

    Atom netSupported( d->findAtom( _NET_SUPPORTED ) );
    Atom searched( d->findAtom( atom ) );

    Atom type;
    int format;
    unsigned char *data;
    unsigned long count;
    unsigned long after;

    int length = 32768;
    while( true )
    {

        // get window property
        const bool success( XGetWindowProperty(
            d->display(), appRootWindow(),
            netSupported, 0l, length,
            false, XA_ATOM, &type,
            &format, &count, &after, &data) == Success );

        if( !success )
        {
            Debug::Throw(debugLevel) << "X11Util::isSupported - XGetProperty failed" << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    if( type != XA_ATOM || format != 32 || count <= 0 || data == None )
    {
        Debug::Throw(debugLevel) << "X11Util::isSupported - incorrect output for XGetProperty" << endl;
        if( data ) XFree( data );
        return false;
    }

    Debug::Throw(debugLevel) << "X11Util::isSupported - count: " << count << endl;
    long *states = (long *) data;
    bool found = false;
    static bool first( true );
    for( unsigned long i = 0; i<count; i++ )
    {

        if( first && Debug::level() >= debugLevel )
        {
            char* data = XGetAtomName( d->display(), (Atom) states[i]);
            Debug::Throw(debugLevel) << "X11Util::isSupported - supported: " << data << endl;
            if ( data ) XFree( data );
        }

        if( searched == (Atom) states[i] )
        {

            found = true;
            if( !( first && Debug::level() < debugLevel ) ) break;

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

    Debug::Throw(debugLevel) << "X11Util::hasProperty - " << d->atomNames_[atom] << endl;

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom net_wm_state( d->findAtom(_NET_WM_STATE) );
    Atom searched( d->findAtom( atom ) );

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
            d->display(), widget.winId(), net_wm_state,
            0L, length, false,
            XA_ATOM, &type,
            &format, &count, &after,
            &data) == Success );

        if( !success )
        {
            Debug::Throw(debugLevel) << "X11Util::hasProperty - XGetProperty failed" << endl;
            return false;
        }

        if( format != 32 )
        {
            Debug::Throw(debugLevel) << "X11Util::hasProperty - wrong format: " << format << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    Debug::Throw(debugLevel) << "X11Util::hasProperty -"
        << " count: " << count
        << endl;

    if( count <= 0 )
    {
        Debug::Throw(debugLevel) << "X11Util::hasProperty - wrong count: " << count << endl;
        return false;
    }

    if( data == None )
    {
        Debug::Throw(debugLevel) << "X11Util::hasProperty - wrong data." << endl;
        return false;
    }

    long *states = (long *) data;
    for( unsigned long i = 0; i<count; i++ )
    {

        if( Debug::level() >= debugLevel )
        {
            char* data = XGetAtomName( d->display(), (Atom) states[i] );
            Debug::Throw(debugLevel) << "X11Util::hasProperty - found: " << data << endl;
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
WId X11Util::appRootWindow( void ) const
{
    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    return DefaultRootWindow( d->display() );
    #else
    return 0;
    #endif
}

//_______________________________________________________
unsigned long X11Util::cardinal( const WId& wid, const Atoms& atom )
{

    Debug::Throw( "X11Util::cardinal" );

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom searched( d->findAtom(atom) );
    Atom type;
    int format;
    unsigned char *data;

    unsigned long n, left;
    XGetWindowProperty(
        d->display(), wid, searched,
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

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // check
    if( !isSupported( _NET_WM_MOVERESIZE ) ) return false;

    Atom net_wm_moveresize( d->findAtom( _NET_WM_MOVERESIZE ) );

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.message_type = net_wm_moveresize;
    event.xclient.display = d->display();
    event.xclient.window = widget.winId();
    event.xclient.format = 32;
    event.xclient.data.l[0] = position.x();
    event.xclient.data.l[1] = position.y();
    event.xclient.data.l[2] = direction;
    event.xclient.data.l[3] = button;
    event.xclient.data.l[4] = 0;
    XUngrabPointer( d->display(), CurrentTime );
    XSendEvent( d->display(), appRootWindow(), false, netWMSendEventMask, &event);
    return true;

    #else
    return false;
    #endif
}

//________________________________________________________________________
bool X11Util::changeProperty( const QWidget& widget, const Atoms& atom, bool value )
{
    return widget.isHidden()  ?
        _changeProperty( widget, atom, value ):
        _requestPropertyChange( widget, atom, value );
}

//________________________________________________________________________
bool X11Util::changeProperty( const QWidget& widget, const Atoms& atom, const unsigned char* data, int size )
{
    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    Atom searched( d->findAtom( atom ) );
    XChangeProperty( d->display(), widget.winId(), searched, XA_CARDINAL, 32, PropModeReplace, data, size );

    return true;
    #endif

    return false;

}

//________________________________________________________________________
bool X11Util::_changeProperty( const QWidget& widget, const Atoms& atom, bool state )
{

    Debug::Throw(debugLevel)
        << "X11Util::_changeProperty -"
        << " winId: " << widget.winId()
        << " atom: " << d->atomNames_[atom]
        << " state: " << state
        << endl;

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom net_wm_state( d->findAtom(_NET_WM_STATE) );
    Atom searched( d->findAtom( atom ) );

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
            d->display(), widget.winId(), net_wm_state,
            0L, length, false,
            XA_ATOM, &type,
            &format, &count, &after,
            &data ) == Success );

        if( !success )
        {
            Debug::Throw(debugLevel) << "X11Util::_changeProperty - XGetProperty failed" << endl;
            return false;
        }

        if( after == 0 ) break;

        XFree( data );
        length *= 2;
        continue;

    }

    Debug::Throw(debugLevel) << "X11Util::_changeProperty -"
        << " count: " << count
        << endl;

    bool found( false );
    Atom *states = reinterpret_cast<Atom*>(data);
    for( unsigned long i = 0; i<count; i++ )
    {

        if( Debug::level() >= debugLevel )
        {
            char* data = XGetAtomName( d->display(), states[i] );
            Debug::Throw(debugLevel) << "X11Util::_changeProperty - found: " << data << endl;
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
    XChangeProperty( d->display(), widget.winId(), net_wm_state, XA_ATOM, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(atoms.constData()), atoms.size() );

    if ( data ) XFree( data );
    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool X11Util::_requestPropertyChange( const QWidget& widget, const Atoms& atom, bool value )
{

    Debug::Throw(debugLevel) << "X11Util::_requestPropertyChange - "
        << " winId: " << widget.winId()
        << " atom: " << d->atomNames_[atom]
        << " state: " << value
        << endl;

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom net_wm_state( d->findAtom(_NET_WM_STATE) );
    Atom requested( d->findAtom( atom ) );

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.message_type = net_wm_state;
    event.xclient.display = d->display();
    event.xclient.window = widget.winId();
    event.xclient.format = 32;
    event.xclient.data.l[0] = (value) ? 1 : 0;
    event.xclient.data.l[1] = requested;
    event.xclient.data.l[2] = 0l;
    event.xclient.data.l[3] = 0l;
    event.xclient.data.l[4] = 0l;

    XSendEvent( d->display(), appRootWindow(), false, netWMSendEventMask, &event);

    return true;

    #endif

    return false;
}

//________________________________________________________________________
bool X11Util::_changeCardinal( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

    Debug::Throw( "X11Util::_changeCardinal.\n" );

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom searched( d->findAtom( atom ) );

    XChangeProperty( d->display(), widget.winId(), searched, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char *>(&value), 1 );
    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool X11Util::_requestCardinalChange( const QWidget& widget, const Atoms& atom, const unsigned long& value )
{

    Debug::Throw( "X11Util::_requestChangeCardinal.\n" );

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    Atom requested( d->findAtom( atom ) );

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.message_type = requested;
    event.xclient.display = d->display();
    event.xclient.window = widget.winId();
    event.xclient.format = 32;
    event.xclient.data.l[0] = value;
    event.xclient.data.l[1] = 0l;
    event.xclient.data.l[2] = 0l;
    event.xclient.data.l[3] = 0l;
    event.xclient.data.l[4] = 0l;

    XSendEvent( d->display(), appRootWindow(), false, netWMSendEventMask, &event);

    return true;
    #endif

    return false;
}

//________________________________________________________
void X11Util::printWindowState( const QWidget& widget )
{

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    Atom net_wm_state( d->findAtom(_NET_WM_STATE) );

    Atom type;
    int format;
    unsigned char *data;
    unsigned long count;
    unsigned long unused;

    // get window property
    if( XGetWindowProperty(
        d->display(), widget.winId(), net_wm_state,
        0L, 2048L, false,
        XA_ATOM, &type,
        &format, &count, &unused,
        &data) != Success )
    {
        Debug::Throw(debugLevel) << "X11Util::printWindowState - XGetProperty failed" << endl;
        return;
    }

    Debug::Throw(debugLevel) << "X11Util::printWindowState -"
        << " count: " << count
        << " unused: " << unused
        << endl;

    Atom* states = reinterpret_cast<Atom*>(data);
    Debug::Throw(0) << "X11Util::printWindowState - ";
    for( unsigned long i = 0; i<count; i++ )
    {

        char* data = XGetAtomName( d->display(), states[i] );
        Debug::Throw(0) << " " << data;
        if ( data ) XFree( data );

    }

    Debug::Throw(0) << endl;
    #endif

}

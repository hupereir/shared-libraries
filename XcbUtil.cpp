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

#include "XcbUtil.h"

#include "Debug.h"

#include <QList>

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif

#if HAVE_X11
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#endif

//________________________________________________________________________
static int debugLevel = 1;

//________________________________________________________________________
#if HAVE_X11
union ClientMessageBuffer
{
    char _buffer[32];
    xcb_client_message_event_t event;

    //! constructor
    ClientMessageBuffer( void )
    { memset( this, 0, sizeof( ClientMessageBuffer ) ); }
};
#endif

//________________________________________________________________________
class XcbUtil::Private
{

    public:

    //! constructor
    Private( void );

    #if HAVE_X11

    //! xcb connection
    xcb_connection_t* connection( void );

    //! get screen matching a given number
    xcb_screen_t* defaultScreen( void );

    //! get screen matching a given number
    xcb_screen_t* screen( int );

    //! root window
    xcb_window_t appRootWindow( void );

    //! find atom
    xcb_atom_t* atom( const QString& );

    //! find atom
    xcb_atom_t* atom( AtomId );

    #endif

    //! atom names
    typedef QHash<AtomId, QString> AtomNameMap;
    AtomNameMap atomNames_;

    //! supported atoms
    typedef QHash<AtomId, bool > SupportedAtomHash;
    SupportedAtomHash supportedAtomId_;

    //! default screen number
    int defaultScreenNumber_;

    private:

    #if HAVE_X11

    //! xcb connection
    xcb_connection_t* connection_;

    //! default screen
    xcb_screen_t* defaultScreen_;

    //! root window
    xcb_window_t appRootWindow_;

    //! atom names map
    typedef QHash<QString, xcb_atom_t> NamedAtomHash;
    NamedAtomHash namedAtomId_;

    //! atom map
    typedef QHash<AtomId, xcb_atom_t> AtomHash;
    AtomHash atoms_;

    #endif
};

//________________________________________________________________________
XcbUtil::Private::Private( void )
{

    #if HAVE_X11
    connection_ = 0;
    defaultScreenNumber_ = 0;
    defaultScreen_ = 0;
    appRootWindow_ = 0;
    #endif
    atomNames_[WM_STATE] = "WM_STATE";

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
    atomNames_[_NET_WM_PID] = "_NET_WM_PID";
    atomNames_[_NET_WM_ICON] = "_NET_WM_ICON";
    atomNames_[_KDE_NET_WM_BLUR_BEHIND_REGION] = "_KDE_NET_WM_BLUR_BEHIND_REGION";

}


#if HAVE_X11
//_______________________________________________________
xcb_connection_t* XcbUtil::Private::connection( void )
{
    if( !connection_ )
    {

        // get display
        #if QT_VERSION >= 0x050000
        QPlatformNativeInterface *native = qApp->platformNativeInterface();
        Display *display = reinterpret_cast<Display*>(native->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen()) );
        #else
        Display* display = QX11Info::display();
        #endif

        // get matching xcb connection
        if( !display ) return 0;

        // assign
        connection_ = XGetXCBConnection( display );

        // also get default screen
        defaultScreenNumber_ = DefaultScreen( display );

    }

    return connection_;

}

//_______________________________________________________
xcb_screen_t* XcbUtil::Private::screen( int screenNumber )
{

    for( xcb_screen_iterator_t iter = xcb_setup_roots_iterator( xcb_get_setup( connection() ) ); iter.rem; --screenNumber, xcb_screen_next( &iter ) )
    { if( screenNumber == 0 ) return iter.data; }

    return 0x0;

}

//_______________________________________________________
xcb_screen_t* XcbUtil::Private::defaultScreen( void )
{
    if( !defaultScreen_ )
    {
        connection();
        defaultScreen_ = screen( defaultScreenNumber_ );
    }

    return defaultScreen_;
}

//_______________________________________________________
xcb_window_t XcbUtil::Private::appRootWindow( void )
{
    if( !appRootWindow_ && defaultScreen() ) { appRootWindow_ = defaultScreen()->root; }
    return appRootWindow_;
}

//________________________________________________________________________
xcb_atom_t* XcbUtil::Private::atom( const QString& atomName )
{

    Debug::Throw( "XcbUtil::Private::atom.\n" );

    // find atom in map
    NamedAtomHash::iterator iter( namedAtomId_.find( atomName ) );
    if( iter != namedAtomId_.end() ) return &iter.value();

    // create atom if not found
    xcb_intern_atom_cookie_t cookie( xcb_intern_atom( connection(), false, atomName.size(), qPrintable( atomName ) ) );
    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, 0x0 ) );
    xcb_atom_t out = reply ? reply->atom:0;
    return &(namedAtomId_[atomName] = out);

}

//________________________________________________________________________
xcb_atom_t* XcbUtil::Private::atom( AtomId atom )
{

    Debug::Throw( "XcbUtil::Private::atom.\n" );

    // find atom in map
    AtomHash::iterator iter( atoms_.find( atom ) );
    if( iter != atoms_.end() ) return &iter.value();

    // create atom if not found
    xcb_intern_atom_cookie_t cookie( xcb_intern_atom( connection(), false, atomNames_[atom].size(), qPrintable( atomNames_[atom] ) ) );
    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, 0x0 ) );
    xcb_atom_t out = reply ? reply->atom:0;
    return &(atoms_[atom] = out);

}

#endif


//________________________________________________________________________
XcbUtil& XcbUtil::get( void )
{
    static XcbUtil singleton;
    return singleton;
}

//________________________________________________________________________
XcbUtil::XcbUtil( void )
{
    Debug::Throw( "XcbUtil::XcbUtil" );
    d = new Private();
}

//________________________________________________________________________
XcbUtil::~XcbUtil( void )
{ delete d; }

//________________________________________________________________________
int XcbUtil::defaultScreenNumber( void ) const
{
    #if HAVE_X11
    d->connection();
    #endif
    return d->defaultScreenNumber_;
}

//_______________________________________________________
WId XcbUtil::appRootWindow( void ) const
{
    #if HAVE_X11
    return d->appRootWindow();
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_connection( void ) const
{
    #if HAVE_X11
    return Qt::HANDLE( d->connection() );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( const QString& atomName ) const
{
    #if HAVE_X11
    return Qt::HANDLE( d->atom( atomName ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( AtomId atom ) const
{
    #if HAVE_X11
    return Qt::HANDLE( d->atom( atom ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
bool XcbUtil::isSupported( AtomId atomId ) const
{

    #if HAVE_X11
    Debug::Throw( debugLevel ) << "XcbUtil::isSupported - " << d->atomNames_[atomId] << endl;

    Private::SupportedAtomHash::const_iterator iter( d->supportedAtomId_.find( atomId ) );
    if( iter != d->supportedAtomId_.end() )
    {
        Debug::Throw() << "XcbUtil::isSupported - " << d->atomNames_[atomId] << (iter.value() ? " true ":" false ") << endl;
        return iter.value();
    }

    bool found( false );
    xcb_atom_t netSupported( *d->atom( _NET_SUPPORTED ) );
    xcb_atom_t searched( *d->atom( atomId ) );

    uint32_t offset(0);
    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, d->appRootWindow(), netSupported, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, 0x0 ) );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::isSupported - XGetProperty failed" << endl;
            return false;
        }

        // cast atom
        const xcb_atom_t current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.data() ))[0] );
        if( searched == current )
        {

            found = true;
            break;

        } else if( reply->bytes_after == 0 ) {

            break;

        } else ++offset;

    }

    d->supportedAtomId_[atomId] = found;

    return found;

    #endif

    return false;

}

//________________________________________________________________________
bool XcbUtil::hasState( WId window, AtomId atomId ) const
{

    Debug::Throw(debugLevel) << "XcbUtil::hasState - " << d->atomNames_[atomId] << endl;

    #if HAVE_X11

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atomId ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atomId ) );

    uint32_t offset(0);
    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, 0x0 ) );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::hasState - XGetProperty failed" << endl;
            return false;
        }

        // cast atom
        const xcb_atom_t current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.data() ))[0] );

        if( searched == current ) return true;
        else if( reply->bytes_after == 0 ) return false;
        else ++offset;

    }

    #endif
    return false;

}

//________________________________________________________________________
void XcbUtil::printState( WId window ) const
{

    Debug::Throw(debugLevel, "XcbUtil::printState.\n" );

    #if HAVE_X11

    // make sure atom is supported
    if( !isSupported( _NET_WM_STATE ) ) return;
    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );

    uint32_t offset(0);
    QStringList atomNames;
    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, 0x0 ) );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::hasState - XGetProperty failed" << endl;
            return;
        }

        // cast atom
        const xcb_atom_t current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.data() ))[0] );
        {

            xcb_get_atom_name_cookie_t cookie( xcb_get_atom_name( d->connection(), current ) );
            XcbUtil::ScopedPointer<xcb_get_atom_name_reply_t> reply( xcb_get_atom_name_reply( d->connection(), cookie, 0 ) );
            if( reply )
            { atomNames.append( QString( QByteArray( xcb_get_atom_name_name( reply.data() ), xcb_get_atom_name_name_length( reply.data() ) ) ) ); }

        }

        if( reply->bytes_after == 0 ) return;
        else ++offset;

    }

    Debug::Throw(0) << "XcbUtil::printWindowState: " << atomNames.join( " " ) << endl;

    #endif
    return;

}

//________________________________________________________________________
uint32_t XcbUtil::cardinal( WId window, AtomId atom ) const
{
    Debug::Throw( "XcbUtil::cardinal" );

    #if HAVE_X11

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t searched( *d->atom(atom) );
    xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, window, searched, XCB_ATOM_CARDINAL, 0, 1 ) );
    ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, 0x0 ) );
    if( !( reply && xcb_get_property_value_length( reply.data() ) > 0 ) ) return 0;

    return reinterpret_cast<uint32_t*>(xcb_get_property_value( reply.data() ) )[0];

    #else
    return 0;
    #endif

}

//________________________________________________________________________
bool XcbUtil::changeState( QWidget* widget, AtomId atom, bool value ) const
{

    return widget->isHidden()  ?
        _changeState( widget, atom, value ):
        _requestStateChange( widget, atom, value );
}

//________________________________________________________________________
bool XcbUtil::changeCardinal( QWidget* widget, AtomId atom, uint32_t value ) const
{
    return widget->isHidden() ?
        _changeCardinal( widget, atom, value ):
        _requestCardinalChange( widget, atom, value );
}

//________________________________________________________________________
bool XcbUtil::moveResizeWidget(
    QWidget* widget,
    const QPoint& position,
    const XcbUtil::Direction& direction,
    Qt::MouseButton button )
{

    Debug::Throw() << "XcbUtil::moveResizeWidget - (" << position.x() << "," << position.y() << ")" << endl;

    if( !widget->isWindow() ) return false;

    #if HAVE_X11

    // check
    if( !isSupported( _NET_WM_MOVERESIZE ) ) return false;

    xcb_atom_t moveWMResize( *d->atom( _NET_WM_MOVERESIZE ) );

    // create event
    ClientMessageBuffer buffer;

    xcb_client_message_event_t *event = &buffer.event;
    event->response_type = XCB_CLIENT_MESSAGE;
    event->type = moveWMResize;
    event->format = 32;
    event->window = widget->winId();
    event->data.data32[0] = position.x();
    event->data.data32[1] = position.y();
    event->data.data32[2] = direction;
    event->data.data32[3] = button;
    event->data.data32[4] = 0;

    xcb_ungrab_pointer( d->connection(), XCB_TIME_CURRENT_TIME );
    xcb_send_event( d->connection(), false, appRootWindow(),
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char*)event );

    xcb_flush( d->connection() );

    return true;

    #else
    return false;
    #endif
}

//________________________________________________________________________
bool XcbUtil::_changeState( QWidget* widget, AtomId atom, bool state ) const
{

    Debug::Throw()
        << "XcbUtil::_changeState -"
        << " winId: " << widget->winId()
        << " atom: " << d->atomNames_[atom]
        << " state: " << state
        << endl;

    #if HAVE_X11

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atom ) );

    QVector<xcb_atom_t> atoms;
    uint32_t offset(0);
    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, widget->winId(), netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, 0x0 ) );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::hasState - XGetProperty failed" << endl;
            return false;
        }

        // cast atom and append
        xcb_atom_t current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.data() ))[0] );
        atoms.append( current );

        if( reply->bytes_after == 0 ) break;
        else ++offset;

    }


    // update list of atoms based on requested state
    if( state )
    {
        if( atoms.contains( searched ) ) return true;
        else atoms.append( searched );

    } else {

        int index = atoms.indexOf( searched );
        if( index >= 0 ) atoms.remove( index );
        else return true;

    }

    // change property
    xcb_change_property( d->connection(), XCB_PROP_MODE_REPLACE, widget->winId(), netWMState, XCB_ATOM_ATOM, 32, atoms.size(), reinterpret_cast<const void*>( atoms.constData() ) );
    xcb_flush( d->connection() );

    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_requestStateChange( QWidget* widget, AtomId atom, bool value ) const
{

    Debug::Throw(debugLevel) << "XcbUtil::_requestStateChange - "
        << " winId: " << widget->winId()
        << " atom: " << d->atomNames_[atom]
        << " state: " << value
        << endl;

    #if HAVE_X11

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t requested( *d->atom( atom ) );

    // create event
    ClientMessageBuffer buffer;

    xcb_client_message_event_t *event = &buffer.event;
    event->response_type = XCB_CLIENT_MESSAGE;
    event->type = netWMState;
    event->format = 32;
    event->window =  widget->window()->winId();
    event->data.data32[0] = value ? 1:0;
    event->data.data32[1] = requested;
    event->data.data32[2] = 0L;
    event->data.data32[3] = 0L;
    event->data.data32[4] = 0L;

    xcb_send_event( d->connection(), false, appRootWindow(), XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char*)event );

    return true;

    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_changeCardinal( QWidget* widget, AtomId atom, uint32_t value ) const
{

    Debug::Throw( "XcbUtil::_changeCardinal.\n" );

    #if HAVE_X11

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t searched( *d->atom( atom ) );
    xcb_change_property( d->connection(), XCB_PROP_MODE_REPLACE, widget->winId(), searched, XCB_ATOM_CARDINAL, 32, 1, &value );
    xcb_flush( d->connection() );

    return true;
    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_requestCardinalChange( QWidget* widget, AtomId atom, uint32_t value ) const
{

    Debug::Throw( "XcbUtil::_requestChangeCardinal.\n" );

    #if HAVE_X11

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t requested( *d->atom( atom ) );

    // create event
    ClientMessageBuffer buffer;

    xcb_client_message_event_t *event = &buffer.event;
    event->response_type = XCB_CLIENT_MESSAGE;
    event->type = requested;
    event->format = 32;
    event->window = widget->winId();
    event->data.data32[0] = value;
    event->data.data32[1] = 0l;
    event->data.data32[2] = 0l;
    event->data.data32[3] = 0l;
    event->data.data32[4] = 0l;

    xcb_send_event( d->connection(), false, appRootWindow(), XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (const char*)event );

    return true;
    #endif

    return false;
}

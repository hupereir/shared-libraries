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
#include <QWindow>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif

#if HAVE_XCB
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#endif

//________________________________________________________________________
static int debugLevel = 1;

//________________________________________________________________________
class XcbUtil::Private
{

    public:

    //* constructor
    Private( void );

    #if HAVE_XCB

    //* xcb connection
    xcb_connection_t* connection( void );

    //* get screen matching a given number
    xcb_screen_t* defaultScreen( void );

    //* get screen matching a given number
    xcb_screen_t* screen( int );

    //* root window
    xcb_window_t appRootWindow( void );

    //* find atom
    xcb_atom_t* atom( const QString& );

    //* find atom
    xcb_atom_t* atom( AtomId );

    #endif

    //* atom names
    typedef QHash<AtomId, QString> AtomNameMap;
    AtomNameMap atomNames_;

    //* supported atoms
    typedef QHash<AtomId, bool > SupportedAtomHash;
    SupportedAtomHash supportedAtomId_;

    //* default screen number
    int defaultScreenNumber_ = 0;

    private:

    #if HAVE_XCB

    //* xcb connection
    xcb_connection_t* connection_ = nullptr;

    //* default screen
    xcb_screen_t* defaultScreen_ = nullptr;

    //* root window
    xcb_window_t appRootWindow_ = 0;

    //* atom names map
    typedef QHash<QString, xcb_atom_t> NamedAtomHash;
    NamedAtomHash namedAtomId_;

    //* atom map
    typedef QHash<AtomId, xcb_atom_t> AtomHash;
    AtomHash atoms_;

    #endif
};

//________________________________________________________________________
XcbUtil::Private::Private( void )
{

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
    atomNames_[_NET_WM_NAME] = "_NET_WM_NAME";
    atomNames_[_KDE_NET_WM_BLUR_BEHIND_REGION] = "_KDE_NET_WM_BLUR_BEHIND_REGION";

}


#if HAVE_XCB
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

    return nullptr;

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
    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, nullptr ) );
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
    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, nullptr ) );
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
XcbUtil::XcbUtil( void ):
    d( new Private )
{ Debug::Throw( "XcbUtil::XcbUtil.\n" ); }

//________________________________________________________________________
XcbUtil::~XcbUtil( void )
{}

//________________________________________________________________________
int XcbUtil::defaultScreenNumber( void ) const
{
    #if HAVE_XCB
    d->connection();
    #endif
    return d->defaultScreenNumber_;
}

//_______________________________________________________
WId XcbUtil::appRootWindow( void ) const
{
    #if HAVE_XCB
    return d->appRootWindow();
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_connection( void ) const
{
    #if HAVE_XCB
    return Qt::HANDLE( d->connection() );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( const QString& atomName ) const
{
    #if HAVE_XCB
    return Qt::HANDLE( d->atom( atomName ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( AtomId atom ) const
{
    #if HAVE_XCB
    return Qt::HANDLE( d->atom( atom ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
bool XcbUtil::isSupported( AtomId atomId ) const
{

    #if HAVE_XCB
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
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, d->appRootWindow(), netSupported, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
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
bool XcbUtil::isRealWindow( WId window ) const
{
    Debug::Throw(debugLevel) << "XcbUtil::isRealWindow - " << windowIdString( window ) << endl;

    #if HAVE_XCB

    xcb_connection_t* connection( d->connection() );
    xcb_atom_t atom( *d->atom( XcbDefines::WM_STATE ) );
    const uint32_t maxLength = std::string().max_size();

    xcb_get_property_cookie_t cookie = xcb_get_property( connection, 0, window, atom,  XCB_ATOM_ANY, 0, maxLength );
    XcbUtil::ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( connection, cookie, nullptr ) );
    return ( reply && xcb_get_property_value_length( reply.data() ) > 0 && reply.data()->type != XCB_ATOM_NONE );

    #else

    return false;

    #endif
}

//________________________________________________________________________
bool XcbUtil::hasState( WId window, AtomId atomId ) const
{
    Debug::Throw(debugLevel) << "XcbUtil::hasState - " << d->atomNames_[atomId] << endl;

    #if HAVE_XCB

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atomId ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atomId ) );

    uint32_t offset(0);
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
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

    #if HAVE_XCB

    // make sure atom is supported
    if( !isSupported( _NET_WM_STATE ) ) return;
    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );

    uint32_t offset(0);
    QStringList atomNames;
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
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

    #if HAVE_XCB

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t searched( *d->atom(atom) );
    xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, window, searched, XCB_ATOM_CARDINAL, 0, 1 ) );
    ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
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
    QPoint position,
    XcbUtil::Direction direction,
    Qt::MouseButton button )
{

    Debug::Throw() << "XcbUtil::moveResizeWidget - (" << position.x() << "," << position.y() << ")" << endl;

    if( !widget->isWindow() ) return false;

    #if HAVE_XCB

    // check
    if( !isSupported( _NET_WM_MOVERESIZE ) ) return false;

    QPoint localPosition( widget->mapFromGlobal( position ) );

    #if QT_VERSION >= 0x050300
    qreal dpiRatio = 1;
    QWindow* windowHandle = widget->window()->windowHandle();
    if( windowHandle ) dpiRatio = windowHandle->devicePixelRatio();
    else dpiRatio = qApp->devicePixelRatio();

    position.rx()*=dpiRatio;
    position.ry()*=dpiRatio;

    localPosition.rx()*=dpiRatio;
    localPosition.ry()*=dpiRatio;

    #endif

    // button release event
    xcb_button_release_event_t releaseEvent;
    memset(&releaseEvent, 0, sizeof(releaseEvent));

    releaseEvent.response_type = XCB_BUTTON_RELEASE;
    releaseEvent.event =  widget->winId();
    releaseEvent.child = XCB_WINDOW_NONE;
    releaseEvent.root = appRootWindow();
    releaseEvent.event_x = localPosition.x();
    releaseEvent.event_y = localPosition.y();
    releaseEvent.root_x = position.x();
    releaseEvent.root_y = position.y();
    releaseEvent.detail = XCB_BUTTON_INDEX_1;
    releaseEvent.state = XCB_BUTTON_MASK_1;
    releaseEvent.time = XCB_CURRENT_TIME;
    releaseEvent.same_screen = true;
    xcb_send_event( d->connection(), false, widget->winId(), XCB_EVENT_MASK_BUTTON_RELEASE, reinterpret_cast<const char*>(&releaseEvent));

    xcb_ungrab_pointer( d->connection(), XCB_TIME_CURRENT_TIME );

    // move resize event
    xcb_atom_t moveWMResize( *d->atom( _NET_WM_MOVERESIZE ) );
    xcb_client_message_event_t clientMessageEvent;
    memset(&clientMessageEvent, 0, sizeof(clientMessageEvent));

    clientMessageEvent.response_type = XCB_CLIENT_MESSAGE;
    clientMessageEvent.type = moveWMResize;
    clientMessageEvent.format = 32;
    clientMessageEvent.window = widget->winId();
    clientMessageEvent.data.data32[0] = position.x();
    clientMessageEvent.data.data32[1] = position.y();
    clientMessageEvent.data.data32[2] = direction;
    clientMessageEvent.data.data32[3] = button;
    clientMessageEvent.data.data32[4] = 0;

    xcb_send_event( d->connection(), false, appRootWindow(),
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        reinterpret_cast<const char*>(&clientMessageEvent) );

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

    #if HAVE_XCB

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atom ) );

    QVector<xcb_atom_t> atoms;
    uint32_t offset(0);
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, widget->winId(), netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
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

    #if HAVE_XCB

    // make sure atoms are supported
    if( !( isSupported( _NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(_NET_WM_STATE) );
    xcb_atom_t requested( *d->atom( atom ) );

    // create event
    xcb_client_message_event_t clientMessageEvent;
    memset(&clientMessageEvent, 0, sizeof(clientMessageEvent));

    clientMessageEvent.response_type = XCB_CLIENT_MESSAGE;
    clientMessageEvent.type = netWMState;
    clientMessageEvent.format = 32;
    clientMessageEvent.window =  widget->window()->winId();
    clientMessageEvent.data.data32[0] = value ? 1:0;
    clientMessageEvent.data.data32[1] = requested;
    clientMessageEvent.data.data32[2] = 0L;
    clientMessageEvent.data.data32[3] = 0L;
    clientMessageEvent.data.data32[4] = 0L;

    xcb_send_event(
        d->connection(), false, appRootWindow(),
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        reinterpret_cast<const char*>(&clientMessageEvent) );

    return true;

    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_changeCardinal( QWidget* widget, AtomId atom, uint32_t value ) const
{

    Debug::Throw( "XcbUtil::_changeCardinal.\n" );

    #if HAVE_XCB

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

    #if HAVE_XCB

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t requested( *d->atom( atom ) );

    // create event
    xcb_client_message_event_t clientMessageEvent;
    memset(&clientMessageEvent, 0, sizeof(clientMessageEvent));

    clientMessageEvent.response_type = XCB_CLIENT_MESSAGE;
    clientMessageEvent.type = requested;
    clientMessageEvent.format = 32;
    clientMessageEvent.window = widget->winId();
    clientMessageEvent.data.data32[0] = value;
    clientMessageEvent.data.data32[1] = 0l;
    clientMessageEvent.data.data32[2] = 0l;
    clientMessageEvent.data.data32[3] = 0l;
    clientMessageEvent.data.data32[4] = 0l;

    xcb_send_event(
        d->connection(), false, appRootWindow(),
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        reinterpret_cast<const char*>(&clientMessageEvent) );

    return true;
    #endif

    return false;
}

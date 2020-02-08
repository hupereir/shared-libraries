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
#include "CppUtil.h"

#include <QList>

#if WITH_XCB

#include <QWindow>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

#include <QX11Info>

#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#endif

//________________________________________________________________________
class XcbUtil::Private
{

    public:

    //* constructor
    Private();

    #if WITH_XCB

    //* xcb connection
    xcb_connection_t* connection();

    //* get screen matching a given number
    xcb_screen_t* defaultScreen();

    //* get screen matching a given number
    xcb_screen_t* screen( int );

    //* root window
    xcb_window_t appRootWindow();

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

    #if WITH_XCB

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
XcbUtil::Private::Private()
{

    atomNames_ =
    {
        { AtomId::WM_STATE, "WM_STATE" },
        { AtomId::_NET_SUPPORTED, "_NET_SUPPORTED" },
        { AtomId::_NET_CURRENT_DESKTOP, "_NET_CURRENT_DESKTOP" },
        { AtomId::_NET_WM_DESKTOP, "_NET_WM_DESKTOP" },
        { AtomId::_NET_WM_STATE, "_NET_WM_STATE" },
        { AtomId::_NET_WM_STATE_STICKY, "_NET_WM_STATE_STICKY" },
        { AtomId::_NET_WM_STATE_STAYS_ON_TOP, "_NET_WM_STATE_STAYS_ON_TOP" },
        { AtomId::_NET_WM_STATE_ABOVE, "_NET_WM_STATE_ABOVE" },
        { AtomId::_NET_WM_STATE_SKIP_TASKBAR, "_NET_WM_STATE_SKIP_TASKBAR" },
        { AtomId::_NET_WM_STATE_SKIP_PAGER, "_NET_WM_STATE_SKIP_PAGER" },
        { AtomId::_NET_WM_MOVERESIZE, "_NET_WM_MOVERESIZE" },
        { AtomId::_NET_WM_CM, "_NET_WM_CM" },
        { AtomId::_NET_WM_PID, "_NET_WM_PID" },
        { AtomId::_NET_WM_ICON, "_NET_WM_ICON" },
        { AtomId::_NET_WM_NAME, "_NET_WM_NAME" },
        { AtomId::_NET_NUMBER_OF_DESKTOPS, "_NET_NUMBER_OF_DESKTOPS" },
        { AtomId::_KDE_NET_WM_BLUR_BEHIND_REGION, "_KDE_NET_WM_BLUR_BEHIND_REGION" }
    };

}


#if WITH_XCB
//_______________________________________________________
xcb_connection_t* XcbUtil::Private::connection()
{
    if( !connection_ )
    {

        // get display
        auto nativeInterface = qApp->platformNativeInterface();
        auto display = reinterpret_cast<Display*>(nativeInterface->nativeResourceForScreen(QByteArray("display"), QGuiApplication::primaryScreen()) );

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
xcb_screen_t* XcbUtil::Private::defaultScreen()
{
    if( !defaultScreen_ )
    {
        connection();
        defaultScreen_ = screen( defaultScreenNumber_ );
    }

    return defaultScreen_;
}

//_______________________________________________________
xcb_window_t XcbUtil::Private::appRootWindow()
{
    if( !appRootWindow_ && defaultScreen() ) { appRootWindow_ = defaultScreen()->root; }
    return appRootWindow_;
}

//________________________________________________________________________
xcb_atom_t* XcbUtil::Private::atom( const QString& atomName )
{

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

    // find atom in map
    AtomHash::iterator iter( atoms_.find( atom ) );
    if( iter != atoms_.end() ) return &iter.value();

    // create atom if not found
    xcb_intern_atom_cookie_t cookie( xcb_intern_atom( connection(), false, atomNames_[atom].size(), qPrintable( atomNames_[atom] ) ) );    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, nullptr ) );
    xcb_atom_t out = reply ? reply->atom:0;
    return &(atoms_[atom] = out);

}

#endif


//________________________________________________________________________
XcbUtil& XcbUtil::get()
{
    static XcbUtil singleton;
    return singleton;
}

//________________________________________________________________________
XcbUtil::XcbUtil():
    d( new Private )
    {}

//________________________________________________________________________
XcbUtil::~XcbUtil() = default;

//________________________________________________________________________
bool XcbUtil::isX11()
{
    #if WITH_XCB
    static const bool isX11 = QX11Info::isPlatformX11();
    return isX11;
    #endif

    return false;

}

//________________________________________________________________________
int XcbUtil::defaultScreenNumber() const
{
    #if WITH_XCB
    if( isX11() ) d->connection();
    #endif
    return d->defaultScreenNumber_;
}

//_______________________________________________________
WId XcbUtil::appRootWindow() const
{
    #if WITH_XCB
    if( isX11() ) return d->appRootWindow();
    #endif

    return 0;

}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_connection() const
{
    #if WITH_XCB
    return Qt::HANDLE( d->connection() );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( const QString& atomName ) const
{
    #if WITH_XCB
    return Qt::HANDLE( d->atom( atomName ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
Qt::HANDLE XcbUtil::_atom( AtomId atom ) const
{
    #if WITH_XCB
    return Qt::HANDLE( d->atom( atom ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
bool XcbUtil::isSupported( AtomId atomId ) const
{

    #if WITH_XCB
    if( !isX11() ) return false;

    Private::SupportedAtomHash::const_iterator iter( d->supportedAtomId_.find( atomId ) );
    if( iter != d->supportedAtomId_.end() )
    { return iter.value(); }

    bool found( false );
    xcb_atom_t netSupported( *d->atom( AtomId::_NET_SUPPORTED ) );
    xcb_atom_t searched( *d->atom( atomId ) );

    uint32_t offset(0);
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, d->appRootWindow(), netSupported, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
        if( !reply ) return false;

        // cast atom
        auto current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.get() ))[0] );
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

    #if WITH_XCB
    if( !isX11() ) return false;

    xcb_connection_t* connection( d->connection() );
    xcb_atom_t atom( *d->atom( XcbDefines::AtomId::WM_STATE ) );

    xcb_get_property_cookie_t cookie = xcb_get_property( connection, 0, window, atom,  XCB_ATOM_ANY, 0, XcbDefines::MAX_PROP_SIZE );
    XcbUtil::ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( connection, cookie, nullptr ) );
    return ( reply && xcb_get_property_value_length( reply.get() ) > 0 && reply->type != XCB_ATOM_NONE );

    #else

    return false;

    #endif
}

//________________________________________________________________________
bool XcbUtil::hasState( WId window, AtomId atomId ) const
{

    #if WITH_XCB

    // make sure atoms are supported
    if( !( isSupported( AtomId::_NET_WM_STATE ) && isSupported( atomId ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(AtomId::_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atomId ) );

    uint32_t offset(0);
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
        if( !reply ) return false;

        // cast atom
        auto current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.get() ))[0] );

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

    #if WITH_XCB

    // make sure atom is supported
    if( !isSupported( AtomId::_NET_WM_STATE ) ) return;
    xcb_atom_t netWMState( *d->atom(AtomId::_NET_WM_STATE) );

    uint32_t offset(0);
    QStringList atomNames;
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, window, netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
        if( !reply ) return;

        // cast atom
        auto current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.get() ))[0] );
        {

            xcb_get_atom_name_cookie_t cookie( xcb_get_atom_name( d->connection(), current ) );
            XcbUtil::ScopedPointer<xcb_get_atom_name_reply_t> reply( xcb_get_atom_name_reply( d->connection(), cookie, 0 ) );
            if( reply )
            { atomNames.append( QString( QByteArray( xcb_get_atom_name_name( reply.get() ), xcb_get_atom_name_name_length( reply.get() ) ) ) ); }

        }

        if( reply->bytes_after == 0 ) return;
        else ++offset;

    }

    #endif
    return;

}

//________________________________________________________________________
uint32_t XcbUtil::cardinal( WId window, AtomId atom ) const
{

    #if WITH_XCB

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t searched( *d->atom(atom) );
    xcb_get_property_cookie_t cookie( xcb_get_property( d->connection(), 0, window, searched, XCB_ATOM_CARDINAL, 0, 1 ) );
    ScopedPointer<xcb_get_property_reply_t> reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
    if( !( reply && xcb_get_property_value_length( reply.get() ) > 0 ) ) return 0;

    return reinterpret_cast<uint32_t*>(xcb_get_property_value( reply.get() ) )[0];

    #else
    return 0;
    #endif

}

//________________________________________________________________________
QIcon XcbUtil::icon( WId window ) const
{
    #if WITH_XCB

    if( isX11() )
    {
        QIcon result;

        // connection and atom
        auto connection( d->connection() );
        auto atom( *d->atom( AtomId::_NET_WM_ICON ) );

        uint32_t offset(0);
        ScopedPointer<xcb_get_property_reply_t> reply;
        forever
        {

            // width
            uint32_t width(0);
            auto cookie( xcb_get_property( connection, 0, window, atom, XCB_ATOM_CARDINAL, offset, 1 ) );
            reply.reset( xcb_get_property_reply( connection, cookie, nullptr ) );
            if( reply && xcb_get_property_value_length( reply.get() ) > 0 )
            {
                width = reinterpret_cast<uint32_t*>( xcb_get_property_value(reply.get() ) )[0];
                ++offset;

            } else break;

            // height
            uint32_t height(0);
            cookie = xcb_get_property( connection, 0, window, atom, XCB_ATOM_CARDINAL, offset, 1 );
            reply.reset( xcb_get_property_reply( connection, cookie, nullptr ) );
            if( reply && xcb_get_property_value_length( reply.get() ) > 0 )
            {
                height = reinterpret_cast<uint32_t*>( xcb_get_property_value( reply.get() ) )[0];
                ++offset;

            } else break;

            // data
            const uint32_t length( width*height );
            cookie = xcb_get_property( connection, 0, window, atom, XCB_ATOM_CARDINAL, offset, length );
            reply.reset( xcb_get_property_reply( connection, cookie, nullptr ) );
            if( reply && xcb_get_property_value_length( reply.get() ) > 0 )
            {

                // get image data
                const uint32_t* imageData( reinterpret_cast<uint32_t*>( xcb_get_property_value( reply.get() ) ) );

                // create image
                QImage image( width, height, QImage::Format_ARGB32);
                for(int i=0; i<image.sizeInBytes()/4; ++i)
                { ((uint32_t*)image.bits())[i] = imageData[i]; }

                // add to icon
                result.addPixmap( QPixmap::fromImage( image ) );

                // free reply and increment offset
                offset += length;

            } else break;
        }

        return result;

    }

    #endif

    return QIcon();

}

//________________________________________________________________________
bool XcbUtil::changeState( QWidget* widget, AtomId atom, bool value ) const
{
    if( isX11() )
    {

        return widget->isHidden()  ?
            _changeState( widget, atom, value ):
            _requestStateChange( widget, atom, value );

    } else return false;

}

//________________________________________________________________________
bool XcbUtil::changeCardinal( QWidget* widget, AtomId atom, uint32_t value ) const
{
    if( isX11() )
    {

        return widget->isHidden() ?
            _changeCardinal( widget, atom, value ):
            _requestCardinalChange( widget, atom, value );

    } else return false;
}

//________________________________________________________________________
bool XcbUtil::moveResizeWidget(
    QWidget* widget,
    QPoint position,
    XcbUtil::Direction direction,
    Qt::MouseButton button )
{

    #if WITH_XCB

    if( !widget->isWindow() ) return false;
    if( !isX11() ) return false;

    // check
    if( !isSupported( AtomId::_NET_WM_MOVERESIZE ) ) return false;

    auto localPosition( widget->mapFromGlobal( position ) );
    qreal dpiRatio = 1;
    auto windowHandle = widget->window()->windowHandle();
    if( windowHandle ) dpiRatio = windowHandle->devicePixelRatio();
    else dpiRatio = qApp->devicePixelRatio();

    position.rx()*=dpiRatio;
    position.ry()*=dpiRatio;

    localPosition.rx()*=dpiRatio;
    localPosition.ry()*=dpiRatio;

    xcb_ungrab_pointer( d->connection(), XCB_TIME_CURRENT_TIME );

    // move resize event
    auto moveWMResize( *d->atom( AtomId::_NET_WM_MOVERESIZE ) );
    xcb_client_message_event_t clientMessageEvent;
    memset(&clientMessageEvent, 0, sizeof(clientMessageEvent));

    clientMessageEvent.response_type = XCB_CLIENT_MESSAGE;
    clientMessageEvent.type = moveWMResize;
    clientMessageEvent.format = 32;
    clientMessageEvent.window = widget->winId();
    clientMessageEvent.data.data32[0] = position.x();
    clientMessageEvent.data.data32[1] = position.y();
    clientMessageEvent.data.data32[2] = Base::toIntegralType( direction );
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

    #if WITH_XCB

    // make sure atoms are supported
    if( !( isSupported( AtomId::_NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(AtomId::_NET_WM_STATE) );
    xcb_atom_t searched( *d->atom( atom ) );

    QVector<xcb_atom_t> atoms;
    uint32_t offset(0);
    forever
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(  d->connection(), 0, widget->winId(), netWMState, XCB_ATOM_ATOM, offset, 1 ) );
        ScopedPointer<xcb_get_property_reply_t>reply( xcb_get_property_reply( d->connection(), cookie, nullptr ) );
        if( !reply ) return false;

        // cast atom and append
        auto current( reinterpret_cast<xcb_atom_t*>(xcb_get_property_value( reply.get() ))[0] );
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
    auto cookie = xcb_change_property_checked( d->connection(), XCB_PROP_MODE_REPLACE, widget->winId(), netWMState, XCB_ATOM_ATOM, 32, atoms.size(), reinterpret_cast<const void*>( atoms.constData() ) );
    ScopedPointer<xcb_generic_error_t> error( xcb_request_check( d->connection(), cookie ) );
    xcb_flush( d->connection() );

    return !error;
    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_requestStateChange( QWidget* widget, AtomId atom, bool value ) const
{

    #if WITH_XCB

    // make sure atoms are supported
    if( !( isSupported( AtomId::_NET_WM_STATE ) && isSupported( atom ) ) ) return false;

    xcb_atom_t netWMState( *d->atom(AtomId::_NET_WM_STATE) );
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

    #if WITH_XCB

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t searched( *d->atom( atom ) );
    auto cookie = xcb_change_property_checked( d->connection(), XCB_PROP_MODE_REPLACE, widget->winId(), searched, XCB_ATOM_CARDINAL, 32, 1, &value );
    ScopedPointer<xcb_generic_error_t> error( xcb_request_check( d->connection(), cookie ) );
    xcb_flush( d->connection() );
    return !error;
    #endif

    return false;
}

//________________________________________________________________________
bool XcbUtil::_requestCardinalChange( QWidget* widget, AtomId atom, uint32_t value ) const
{

    #if WITH_XCB

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

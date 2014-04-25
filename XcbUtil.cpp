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

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#else
#include <QX11Info>
#endif

#if HAVE_X11
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#endif

//________________________________________________________________________
static int debugLevel = 1;

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
    xcb_atom_t findAtom( Atoms );

    #endif

    XcbUtil::AtomNameMap atomNames_;

    private:

    //! default screen number
    int defaultScreenNumber_;

    #if HAVE_X11

    //! xcb connection
    xcb_connection_t* connection_;

    //! default screen
    xcb_screen_t* defaultScreen_;

    //! root window
    xcb_window_t appRootWindow_;

    //! atom map
    typedef QHash<Atoms, xcb_atom_t> AtomMap;

    //! atoms
    AtomMap atoms_;

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
xcb_atom_t XcbUtil::Private::findAtom( Atoms atom )
{

    Debug::Throw( "XcbUtil::Private::findAtom.\n" );

    // find atom in map
    Private::AtomMap::iterator iter( atoms_.find( atom ) );
    if( iter != atoms_.end() ) return iter.value();

    // create atom if not found
    xcb_intern_atom_cookie_t cookie( xcb_intern_atom( connection(), false, atomNames_[atom].size(), qPrintable( atomNames_[atom] ) ) );
    ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection(), cookie, 0x0 ) );
    xcb_atom_t out = reply ? reply->atom:0;
    atoms_[atom] = out;
    return out;

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
Qt::HANDLE XcbUtil::connection( void ) const
{
    #if HAVE_X11
    return Qt::HANDLE( d->connection() );
    #else
    return 0;
    #endif
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
Qt::HANDLE XcbUtil::findAtom( Atoms atom ) const
{
    #if HAVE_X11
    return Qt::HANDLE( d->findAtom( atom ) );
    #else
    return 0;
    #endif
}

//________________________________________________________________________
bool XcbUtil::isSupported( Atoms atom )
{

    #if HAVE_X11

    Debug::Throw( debugLevel ) << "XcbUtil::isSupported - " << d->atomNames_[atom] << endl;

    SupportedAtomMap::const_iterator iter( supportedAtoms_.find( atom ) );
    if( iter != supportedAtoms_.end() )
    {
        Debug::Throw() << "XcbUtil::isSupported - " << d->atomNames_[atom] << (iter.value() ? " true ":" false ") << endl;
        return iter.value();
    }

    xcb_atom_t netSupported( d->findAtom( _NET_SUPPORTED ) );
    xcb_atom_t searched( d->findAtom( atom ) );
    xcb_get_property_reply_t* reply = 0;
    int length = 32768;

    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(
            d->connection(), 0, d->appRootWindow(),
            netSupported, XCB_ATOM_ATOM, 0, length ) );

        reply = xcb_get_property_reply( d->connection(), cookie, 0x0 );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::isSupported - XGetProperty failed" << endl;
            return false;
        }

        if( reply->bytes_after == 0 ) break;
        free( reply );
        length *= 2;
        continue;

    }

    // TODO: sanitize the times 4 division
    const int count( xcb_get_property_value_length( reply )/4 );
    uint32_t* data( reinterpret_cast<uint32_t*>( xcb_get_property_value( reply ) ) );
    if( reply->type != XCB_ATOM_ATOM || reply->format != 32 || count <= 0 || !data )
    {
        Debug::Throw(debugLevel) << "XcbUtil::isSupported - incorrect output for XGetProperty" << endl;
        free( reply );
        return false;
    }

    Debug::Throw(debugLevel) << "XcbUtil::isSupported - count: " << count << endl;

    bool found = false;
    static bool first( true );
    for( int i = 0; i<count; ++i )
    {

        if( first && Debug::level() >= debugLevel )
        {

            // print all supported atom names
            xcb_get_atom_name_cookie_t cookie( xcb_get_atom_name( d->connection(), (xcb_atom_t) data[i] ) );
            ScopedPointer<xcb_get_atom_name_reply_t> reply( xcb_get_atom_name_reply( d->connection(), cookie, 0 ) );

            if( !reply ) continue;
            Debug::Throw(debugLevel) << "XcbUtil::isSupported - supported: " << xcb_get_atom_name_name( reply.data() ) << endl;

        }

        if( searched == (xcb_atom_t) data[i] )
        {

            found = true;
            if( !( first && Debug::level() < debugLevel ) ) break;

        }

    }

    if( first ) first = false;
    supportedAtoms_[atom] = found;
    free( reply );

    return found;

    #endif

    return false;

}

//________________________________________________________________________
bool XcbUtil::hasProperty( QWidget* widget, Atoms atom )
{

    Debug::Throw(debugLevel) << "XcbUtil::hasProperty - " << d->atomNames_[atom] << endl;

    #if HAVE_X11

    // make sure atom is supported
    if( !isSupported( atom ) ) return false;

    xcb_atom_t netWMState( d->findAtom(_NET_WM_STATE) );
    xcb_atom_t searched( d->findAtom( atom ) );
    xcb_get_property_reply_t* reply = 0;
    int length = 32768;

    while( true )
    {

        xcb_get_property_cookie_t cookie( xcb_get_property(
            d->connection(), 0, widget->winId(), netWMState,
            XCB_ATOM_ATOM, 0, length ) );

        reply = xcb_get_property_reply( d->connection(), cookie, 0x0 );
        if( !reply )
        {
            Debug::Throw(debugLevel) << "XcbUtil::hasProperty - XGetProperty failed" << endl;
            return false;
        }

        if( reply->format != 32 )
        {
            Debug::Throw(debugLevel) << "XcbUtil::hasProperty - wrong format: " << reply->format << endl;
            return false;
        }

        if( reply->bytes_after == 0 ) break;

        free( reply );
        length *= 2;
        continue;

    }

    const int count( xcb_get_property_value_length( reply )/4 );
    Debug::Throw(debugLevel) << "XcbUtil::hasProperty -"
        << " count: " << count
        << endl;

    if( count <= 0 )
    {
        Debug::Throw(debugLevel) << "XcbUtil::hasProperty - wrong count: " << count << endl;
        return false;
    }

    uint32_t* data( reinterpret_cast<uint32_t*>( xcb_get_property_value( reply ) ) );
    if( !data )
    {
        Debug::Throw(debugLevel) << "XcbUtil::hasProperty - wrong data." << endl;
        return false;
    }

    for( unsigned long i = 0; i<count; i++ )
    {

        if( Debug::level() >= debugLevel )
        {
            // print all supported atom names
            xcb_get_atom_name_cookie_t cookie( xcb_get_atom_name( d->connection(), (xcb_atom_t) data[i] ) );
            ScopedPointer<xcb_get_atom_name_reply_t> reply( xcb_get_atom_name_reply( d->connection(), cookie, 0 ) );

            if( !reply ) continue;
            Debug::Throw(debugLevel) << "XcbUtil::isSupported - supported: " << xcb_get_atom_name_name( reply.data() ) << endl;
        }

        if( searched == (xcb_atom_t) data[i] )
        {
            free(reply);
            return true;
        }

    }

    // no match found
    free( reply );
    return false;

    #else
    return false;
    #endif

}

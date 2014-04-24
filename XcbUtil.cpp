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
class XcbUtil::Private
{

    public:

    //! constructor
    Private( void );

    #if HAVE_X11

    //! xcb connection
    xcb_connection_t* connection( void );

    //! xcb connection
    xcb_connection_t* connection_;

    #endif
};

//________________________________________________________________________
XcbUtil::Private::Private( void )
{

    #if HAVE_X11
    connection_ = 0;
    #endif

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
        if( display ) connection_ = XGetXCBConnection( display );
    }

    return connection_;

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

// $Id$

/***************************************************************************
*   Copyright (C) 2006 by Helio Chissini de Castro   *
*   helio@mandriva.com   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

/*!
\file    CompositeEngine.h
\brief   initialize compositing, if available
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include "Debug.h"

#include "BackgroundPixmap.h"
#include "CompositeEngine.h"
#include "XmlOptions.h"

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/extensions/Xrender.h>
#endif

using namespace std;
using namespace TRANSPARENCY;

//_______________________________________________________________
CompositeEngine& CompositeEngine::get( void )
{

    //! singleton
    static CompositeEngine singleton;
    return singleton;

}

//_______________________________________________________________
CompositeEngine::CompositeEngine( void ):

    #ifdef Q_WS_X11
    display_( 0 ),
    visual_( 0 ),
    colormap_( 0 ),
    #endif

    available_( false ),
    enabled_( true ),
    initialized_( false )
{
    Debug::Throw( "CompositeEngine::CompositeEngine.\n" );
    XmlOptions::get().set( "TRANSPARENCY_USE_COMPOSITE", Option("1"), true );
    XmlOptions::get().set( "TRANSPARENCY_USE_BLUR", Option("1"), true );

}


//_______________________________________________________________
void CompositeEngine::initialize( void )
{

    Debug::Throw( "CompositeEngine::initialize\n" );

    if( initialized_ ) return;
    initialized_ = true;

    // reset
    available_ = false;

    // do nothing if compositing is not enabled
    if( !_compositingEnabled() ) return;

    #ifdef Q_WS_X11
    // display
    display_ = XOpenDisplay(0);

    int screen = DefaultScreen(display_);
    int n_visuals;

    XVisualInfo templ;
    templ.screen  = screen;
    templ.depth   = 32;
    templ.c_class = TrueColor;
    XVisualInfo *visual_info = XGetVisualInfo(display_, VisualScreenMask | VisualDepthMask | VisualClassMask, &templ, &n_visuals);

    for (int i = 0; i < n_visuals; ++i)
    {
        XRenderPictFormat *format = XRenderFindVisualFormat(display_, visual_info[i].visual);
        if (format->type == PictTypeDirect && format->direct.alphaMask)
        {
            visual_ = visual_info[i].visual;
            colormap_ = XCreateColormap(display_, RootWindow(display_, screen), visual_, AllocNone);
            available_ = true;
            break;
        }

    }

    // need to close the display if not available
    // to let Qt use its own.
    if( !available_ )
    {
        XCloseDisplay( display_ );
        display_ = 0;
    }
    #endif

    #ifdef Q_WS_WIN
    // on windows,
    // composition mode is available by default
    // TODO: check version to make sure this is true
    available_ = true;
    #endif

}

//_______________________________________________________________
bool CompositeEngine::setEnabled( bool value )
{
    if( enabled_ == value ) return false;
    enabled_ = value;

    // if disabled, reload background pixmap for fake transparency
    if( !value ) { BackgroundPixmap::get().reload(); }

    return true;
}

//_______________________________________________________________
bool CompositeEngine::_compositingEnabled( void ) const
{
    Debug::Throw( "CompositeEngine::_compositingEnabled\n" );

    #ifdef Q_WS_X11
    if( QX11Info::display()) return _compositingEnabled( QX11Info::display() );
    else {
        Debug::Throw( "CompositeEngine::_compositingEnabled - creating dummy display\n" );
        Display* display = XOpenDisplay( 0 );
        bool valid( _compositingEnabled( display ) );
        XCloseDisplay( display );
        return valid;
    }
    #endif

    #ifdef Q_WS_WIN
    // on windows,
    // composition mode is available by default
    // TODO: check version to make sure this is true
    return true;
    #endif

    // on all other systems, return false
    return false;

}

//_______________________________________________________________
#ifdef Q_WS_X11
bool CompositeEngine::_compositingEnabled( Display* display ) const
{
    Debug::Throw( "CompositeEngine::_compositingEnabled (display)\n" );

    char atom_name[ 100 ];
    sprintf( atom_name, "_NET_WM_CM_S%d", DefaultScreen( display ));
    Atom atom( XInternAtom( display, atom_name, false ) );
    return XGetSelectionOwner( display, atom ) != None;

}
#endif

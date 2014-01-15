#ifndef X11ErrorHandler_h
#define X11ErrorHandler_h

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

#include "Counter.h"

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
#include <X11/Xlib.h>
#endif

class X11ErrorHandler: public Counter
{
    public:

    //! constructor
    X11ErrorHandler( void ):
        Counter( "X11ErrorHandler" )
    {

        #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
        oldHandler_ = XSetErrorHandler( ignoreErrors );
        #endif

    }

    //! destructor
    virtual ~X11ErrorHandler( void )
    {
        #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
        XSetErrorHandler( oldHandler_ );
        #endif
    }

    private:

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )
    //! error handler
    static int ignoreErrors (Display *dpy, XErrorEvent *ev)
    { return 0; }

    //! old handler
    XErrorHandler oldHandler_;

    #endif
};

#endif

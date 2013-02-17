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

#include "BackgroundPixmap.h"
#include "Debug.h"
#include "XmlOptions.h"

#include "X11Util.h"
#include "CompositeEngine.h"

#if defined(Q_WS_X11) || defined(Q5_WS_X11)
#include <X11/extensions/Xrender.h>
#endif


namespace TRANSPARENCY
{

    //_______________________________________________________________
    CompositeEngine& CompositeEngine::get( void )
    {

        //! singleton
        static CompositeEngine singleton;
        return singleton;

    }

    //_______________________________________________________________
    CompositeEngine::CompositeEngine( void ):
        available_( false ),
        enabled_( true ),
        initialized_( false )
    {
        Debug::Throw( "CompositeEngine::CompositeEngine.\n" );
        XmlOptions::get().set( "TRANSPARENCY_USE_COMPOSITE", Option("1"), true );
        XmlOptions::get().set( "TRANSPARENCY_USE_BLUR", Option("1"), true );

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
    void CompositeEngine::_initialize( void )
    {

        Debug::Throw( "CompositeEngine::initialize\n" );
        if( !initialized_ )
        {
            initialized_ = true;
            available_ = _compositingEnabled();
        }
    }

    //_______________________________________________________________
    bool CompositeEngine::_compositingEnabled( void ) const
    {
        Debug::Throw( "CompositeEngine::_compositingEnabled\n" );

        #if defined(Q_OS_WIN)
        return true;
        #endif

        #if defined(Q_WS_X11) || defined(Q5_WS_X11)
        Display* display = X11Util::get().display();
        QByteArray buffer;
        QTextStream( &buffer ) << "_NET_WM_CM_S" << DefaultScreen( display );
        Atom atom( XInternAtom( display, buffer.constData(), False) );
        return XGetSelectionOwner( display, atom ) != None;
        #endif

        // on all other systems, return false
        return false;

    }

}

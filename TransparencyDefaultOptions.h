#ifndef TransparencyDefaultOptions_h
#define TransparencyDefaultOptions_h

// $Id$

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA Options::Get().Add( Option("mailto: hugo.pereira@free.fr>
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

#include "Color.h"
#include "XmlOptions.h"

//_____________________________________________________
//! default options installer
void installTransparencyOptions( void )
{

    // foreground
    XmlOptions::get().set<BASE::Color>( "TRANSPARENCY_FOREGROUND_COLOR", QColor( Qt::white ) );
    XmlOptions::get().set<int>( "TRANSPARENCY_FOREGROUND_INTENSITY", 255 );

    // shadow
    XmlOptions::get().set<BASE::Color>( "TRANSPARENCY_SHADOW_COLOR", QColor( Qt::black ) );
    XmlOptions::get().set<int>( "TRANSPARENCY_SHADOW_OFFSET", 1 );
    XmlOptions::get().set<bool>( "TRANSPARENCY_INVERSE_COLORS", false );
    XmlOptions::get().set<bool>( "TRANSPARENCY_USE_GRADIENT", true );

    // tint
    XmlOptions::get().set<BASE::Color>( "TRANSPARENCY_TINT_COLOR", QColor( Qt::white ) );
    XmlOptions::get().set<double>( "TRANSPARENCY_TINT_INTENSITY", 0 );

    // highlight
    XmlOptions::get().set<BASE::Color>( "TRANSPARENCY_HIGHLIGHT_COLOR", QColor( Qt::white ) );
    XmlOptions::get().set<int>( "TRANSPARENCY_HIGHLIGHT_INTENSITY", 0 );

    // overall transparency
    XmlOptions::get().set<int>( "TRANSPARENCY_OPACITY", 255 );

}

#endif

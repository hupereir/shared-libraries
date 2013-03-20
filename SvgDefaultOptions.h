#ifndef SvgDefaultOptions_h
#define SvgDefaultOptions_h

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

#include "XmlOptions.h"
#include "SvgPlasmaInterface.h"

//_____________________________________________________
//! default options installer
void installSvgOptions( void )
{
    XmlOptions::get().setAutoDefault( true );
    XmlOptions::get().keep( "SVG_BACKGROUND" );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-air.svgz" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-dark.svgz", Option::Recordable|Option::Current ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-light.svgz" ) );
    XmlOptions::get().set<bool>( "USE_SVG", true );
    XmlOptions::get().set<bool>( "SVG_USE_PLASMA_INTERFACE", true );
    XmlOptions::get().set<bool>( "SVG_DRAW_OVERLAY", false );
    XmlOptions::get().set<int>( "SVG_PLASMA_IMAGE_PATH", SVG::SvgPlasmaInterface::WidgetBackground );
    XmlOptions::get().setAutoDefault( false );
}

#endif

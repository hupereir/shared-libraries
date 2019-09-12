#ifndef SvgDefaultOptions_h
#define SvgDefaultOptions_h

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

#include "CppUtil.h"
#include "SvgPlasmaInterface.h"
#include "XmlOptions.h"

//_____________________________________________________
//* default options installer
void installSvgOptions()
{
    XmlOptions::get().setAutoDefault( true );
    XmlOptions::get().keep( "SVG_BACKGROUND" );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-air.svgz" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-dark.svgz", Option::Flag::Recordable|Option::Flag::Current ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-light.svgz" ) );
    XmlOptions::get().set<bool>( "USE_SVG", true );
    XmlOptions::get().set<bool>( "SVG_USE_PLASMA_INTERFACE", true );
    XmlOptions::get().set<bool>( "SVG_DRAW_OVERLAY", false );
    XmlOptions::get().set<int>( "SVG_PLASMA_IMAGE_PATH", Base::toIntegralType( Svg::SvgPlasmaInterface::ImagePath::WidgetBackground ) );
    XmlOptions::get().setAutoDefault( false );
}

#endif

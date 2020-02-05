#ifndef TransparencyDefaultOptions_h
#define TransparencyDefaultOptions_h

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

#include "Color.h"
#include "XmlOptions.h"

//_____________________________________________________
//* default options installer
void installTransparencyOptions()
{

    // foreground
    XmlOptions::get().set( "TRANSPARENCY_FOREGROUND_COLOR", Base::Color( Qt::white ) );
    XmlOptions::get().set<int>( QStringLiteral("TRANSPARENCY_FOREGROUND_INTENSITY"), 255 );

    // shadow
    XmlOptions::get().set( "TRANSPARENCY_SHADOW_COLOR", Base::Color( Qt::black ) );
    XmlOptions::get().set<int>( QStringLiteral("TRANSPARENCY_SHADOW_OFFSET"), 1 );
    XmlOptions::get().set<bool>( QStringLiteral("TRANSPARENCY_INVERSE_COLORS"), false );
    XmlOptions::get().set<bool>( QStringLiteral("TRANSPARENCY_USE_GRADIENT"), true );

    // tint
    XmlOptions::get().set( "TRANSPARENCY_TINT_COLOR", Base::Color( Qt::white ) );
    XmlOptions::get().set<double>( QStringLiteral("TRANSPARENCY_TINT_INTENSITY"), 0 );

}

#endif

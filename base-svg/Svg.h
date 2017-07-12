#ifndef Svg_h
#define Svg_h

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

#include "SvgId.h"

#include <QString>
#include <QMap>
#include <QPixmap>
#include <QImage>

//* namespace for icon static name wrappers
namespace Svg
{

    //*@name main panel
    //@{
    static const QString TopLeft="topleft";
    static const QString Top="top";
    static const QString TopRight="topright";
    static const QString Left="left";
    static const QString Center="center";
    static const QString Right="right";
    static const QString BottomRight="bottomright";
    static const QString Bottom="bottom";
    static const QString BottomLeft="bottomleft";
    //@}

    //* overlay
    static const QString Overlay="overlay";

    //*@name main panel margin hints
    //@{
    static const QString MarginLeft="hint-left-margin";
    static const QString MarginRight="hint-right-margin";
    static const QString MarginTop="hint-top-margin";
    static const QString MarginBottom="hint-bottom-margin";
    //@}

    //*@name main panel shadow hints
    //@{
    static const QString ShadowLeft="hint-left-shadow";
    static const QString ShadowRight="hint-right-shadow";
    static const QString ShadowTop="hint-top-shadow";
    static const QString ShadowBottom="hint-bottom-shadow";
    //@}

    //* map size and pixmap
    using PixmapCache = QMap< SvgId, QPixmap >;
    using ImageCache = QMap< SvgId, QImage >;

};

#endif

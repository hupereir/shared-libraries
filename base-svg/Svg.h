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
    static const QString TopLeft=QStringLiteral("topleft");
    static const QString Top=QStringLiteral("top");
    static const QString TopRight=QStringLiteral("topright");
    static const QString Left=QStringLiteral("left");
    static const QString Center=QStringLiteral("center");
    static const QString Right=QStringLiteral("right");
    static const QString BottomRight=QStringLiteral("bottomright");
    static const QString Bottom=QStringLiteral("bottom");
    static const QString BottomLeft=QStringLiteral("bottomleft");
    //@}

    //* overlay
    static const QString Overlay=QStringLiteral("overlay");

    //*@name main panel margin hints
    //@{
    static const QString MarginLeft=QStringLiteral("hint-left-margin");
    static const QString MarginRight=QStringLiteral("hint-right-margin");
    static const QString MarginTop=QStringLiteral("hint-top-margin");
    static const QString MarginBottom=QStringLiteral("hint-bottom-margin");
    //@}

    //*@name main panel shadow hints
    //@{
    static const QString ShadowLeft=QStringLiteral("hint-left-shadow");
    static const QString ShadowRight=QStringLiteral("hint-right-shadow");
    static const QString ShadowTop=QStringLiteral("hint-top-shadow");
    static const QString ShadowBottom=QStringLiteral("hint-bottom-shadow");
    //@}

    //* map size and pixmap
    using PixmapCache = QMap< SvgId, QPixmap >;
    using ImageCache = QMap< SvgId, QImage >;

};

#endif

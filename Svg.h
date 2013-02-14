#ifndef Svg_h
#define Svg_h

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

#include "SvgId.h"

#include <QString>
#include <QMap>
#include <QPixmap>
#include <QImage>

//! namespace for icon static name wrappers
namespace SVG
{

    static const QString TopLeft="topleft";
    static const QString Top="top";
    static const QString TopRight="topright";
    static const QString Left="left";
    static const QString Center="center";
    static const QString Right="right";
    static const QString BottomRight="bottomright";
    static const QString Bottom="bottom";
    static const QString BottomLeft="bottomleft";

    static const QString Overlay="overlay";

    static const QString MarginLeft="hint-left-margin";
    static const QString MarginRight="hint-right-margin";
    static const QString MarginTop="hint-top-margin";
    static const QString MarginBottom="hint-bottom-margin";

    //! map size and pixmap
    typedef QMap< SvgId, QPixmap > PixmapCache;
    typedef QMap< SvgId, QImage > ImageCache;

};

#endif

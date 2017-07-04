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

#include "IconSize.h"

#include <QApplication>
#include <QStyle>

//__________________________________________________________________
IconSize::IconSize( IconSize::Size size )
{

    int iconSize( size );
    if( size == Default )
    { iconSize =  qApp->style()->pixelMetric( QStyle::PM_ToolBarIconSize ); }

    size_.setWidth( iconSize );
    size_.setHeight( iconSize );
}

//______________________________________________________________________
IconSize::Map& IconSize::map()
{
    static Map sizeMap =
    {
        { Default, QObject::tr( "System Default" ) },
        { Small, QObject::tr( "Small (16x16)" ) },
        { Medium, QObject::tr( "Medium (22x22)" ) },
        { Large, QObject::tr( "Large (32x32)" ) },
        { Huge, QObject::tr( "Huge (48x48)" ) },
        { Oversized, QObject::tr( "Oversized (128x128)" ) },
        { Maximum, QObject::tr( "Maximum (256x256)" ) }
    };

    return sizeMap;
}

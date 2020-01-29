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
IconSize::IconSize( IconSize::Size size ):
   size_( get( size ) )
{}

//__________________________________________________________________
QSize IconSize::get( IconSize::Size size )
{

    int iconSize( size );
    if( size == Default )
    { iconSize =  qApp->style()->pixelMetric( QStyle::PM_ToolBarIconSize ); }

    return QSize( iconSize, iconSize );
}

//______________________________________________________________________
const IconSize::Map& IconSize::map()
{
    static const Map sizeMap(
    {
        { Default, QObject::tr( "System Default" ) },
        { Minimum, QObject::tr( "Very Small (8x8)" ) },
        { Small, QObject::tr( "Small (16x16)" ) },
        { Medium, QObject::tr( "Medium (22x22)" ) },
        { Large, QObject::tr( "Large (32x32)" ) },
        { VeryLarge, QObject::tr( "Very Large (48x48)" ) },
        { Huge, QObject::tr( "Huge (64x64)" ) },
        { VeryHuge, QObject::tr( "Very Huge (128x128)" ) },
        { Maximum, QObject::tr( "Maximum (256x256)" ) }
    } );

    return sizeMap;
}

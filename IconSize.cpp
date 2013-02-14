
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

#include "IconSize.h"

#include <QStyle>

//__________________________________________________________________
IconSize::IconSize( IconSize::Size size )
{

    Q_ASSERT( size != Default );
    setWidth( size );
    setHeight( size );

}

//__________________________________________________________________
IconSize::IconSize( QWidget* parent, IconSize::Size size )
{

    int iconSize( size );
    if( size == Default )
    {
        Q_CHECK_PTR( parent );
        iconSize =  parent->style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    }

    setWidth( iconSize );
    setHeight( iconSize );

}

//______________________________________________________________________
IconSize::Map& IconSize::map( void )
{
    static Map sizeMap;
    if( sizeMap.empty() )
    {
        sizeMap.insert( Default, "System Default" );
        sizeMap.insert( Small, "Small (16x16)" );
        sizeMap.insert( Medium, "Medium (22x22)" );
        sizeMap.insert( Large, "Large (32x32)" );
        sizeMap.insert( Huge, "Huge (48x48)" );
    }

    return sizeMap;
}

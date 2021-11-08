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

#include "ColorOptionModel.h"
#include "CppUtil.h"
#include "IconSize.h"
#include "Pixmap.h"


#include <QPainter>
#include <QPalette>

//_______________________________________________
ColorOptionModel::IconCache& ColorOptionModel::_icons()
{
    static IconCache singleton;
    return singleton;
}

//__________________________________________________________________
QVariant ColorOptionModel::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // retrieve associated color
    auto color( get( index ).second.get<Base::Color>() );

    if( role == Qt::DisplayRole && index.column() == Value ) return color.get().name();
    else if( role == Qt::DecorationRole && index.column() == Value ) return _icon( color );
    else return OptionModel::data( index, role );

}

//______________________________________________________
const QIcon& ColorOptionModel::_icon( const Base::Color& color )
{

    auto iter( _icons().lowerBound( color ) );
    if( iter != _icons().end() && Base::areEquivalent( color, iter.key() ) ) return iter.value();
    else {


        Pixmap pixmap( IconSize::get( IconSize::Large ), Pixmap::Flag::Transparent );
        QRect rect( QPoint(0,0), pixmap.size()/pixmap.devicePixelRatio() );
        rect.adjust( 2, 2, -2, -2 );

        QPainter painter( &pixmap );
        painter.setRenderHints(QPainter::Antialiasing );

        painter.setPen( Qt::NoPen );
        painter.setBrush( color.get() );
        painter.drawEllipse( rect );

        painter.setPen( QPalette().color( QPalette::Text ) );
        painter.setBrush( Qt::NoBrush );
        painter.drawEllipse( rect );

        return _icons().insert( iter, color, QIcon(pixmap) ).value();

    }

}

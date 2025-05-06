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

#include "LineEditorButton.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOptionButton>

//____________________________________________________________
LineEditorButton::LineEditorButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( QStringLiteral("LineEditorButton") )
{

    QStyleOptionButton option;
    option.initFrom( this );
    const int iconSize( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    setIconSize( QSize( iconSize, iconSize ) );
}

//____________________________________________________________
QSize LineEditorButton::sizeHint() const
{
    QStyleOptionButton option;
    option.initFrom( this );

    const int iconSize( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    const int margin( 2 );
    const int dimension = iconSize + 2*margin;
    return QSize( dimension, dimension );
}

//____________________________________________________________
void LineEditorButton::setVisible(bool value)
{
    const bool changed = (value != isVisible());
    QToolButton::setVisible(value);
    if( changed ) emit visibilityChanged(value);
}

//____________________________________________________________
void LineEditorButton::paintEvent( QPaintEvent* event )
{
    if( icon().isNull() ) return;

    QStyleOptionButton option;
    option.initFrom( this );

    const int iconWidth( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    const QSize iconSize( iconWidth, iconWidth );
    const auto pixmap( icon().pixmap( iconSize ) );

    const auto rect( this->rect() );
    const QRect iconRect( QPoint( rect.x() + (rect.width() - iconWidth)/2, rect.y() + (rect.height() - iconWidth)/2 ), iconSize );

    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.drawPixmap( iconRect, pixmap );
}

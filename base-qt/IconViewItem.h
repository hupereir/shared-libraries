#ifndef IconViewItem_h
#define IconViewItem_h

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

#include "Counter.h"
#include "CustomPixmap.h"

#include <QRect>

#include <QPainter>
#include <QStyleOption>

//* graphics item
class IconViewItem final: private Base::Counter<IconViewItem>
{

    public:

    //* constructor
    explicit IconViewItem():
        Counter( QStringLiteral("IconView::Item") )
    {}

    //*@name accessors
    //@{

    //* icon
    const CustomPixmap& pixmap() const
    { return pixmap_; }

    //* text
    const QString& text() const
    { return text_; }

    //* position
    QPoint position() const
    { return position_; }

    //* row
    int row() const
    { return row_; }

    //* column
    int column() const
    { return column_; }

    //* bounding rect
    QRect boundingRect() const;

    //@}

    //*@name modifiers
    //@{

    //* set icon
    void setPixmap( const CustomPixmap& pixmap )
    {
        const bool changed( pixmap_.size() != pixmap.size() );
        pixmap_ = pixmap;
        if( changed ) dirty_ = true;
    }

    //* set text
    void setText( const QString& text )
    {
        if( text_ == text ) return;
        text_ = text;
        dirty_ = true;
    }

    //* set position
    void setPosition( const QPoint& position )
    { position_ = position; }

    //* set location
    void setLocation( int row, int column )
    {
        row_ = row;
        column_ = column;
    }

    //@}

    //* item map
    using Map = QMap<int, IconViewItem>;

    //* item list
    using List = QList<IconViewItem>;

    //* paint
    void paint( QPainter*, const QStyleOption*, QWidget* ) const;

    //*@name metrics
    //@{
    static const int margin;
    static const int spacing;
    static const int maxTextWidth;
    //@}

    protected:

    //* update bounding rect
    void _updateBoundingRect();

    private:

    //* dirty
    bool dirty_ = true;

    //* pixmap
    CustomPixmap pixmap_;

    //* text
    QString text_;

    //* bounding rect
    QRect boundingRect_;

    //* position
    QPoint position_;

    //* row and column index
    int row_ = -1;

    //* column
    int column_ = -1;

};

#endif

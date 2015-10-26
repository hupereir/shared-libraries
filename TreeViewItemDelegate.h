#ifndef TreeViewItemDelegate_h
#define TreeViewItemDelegate_h

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

#include <QStyledItemDelegate>

class TreeViewItemDelegate: public QStyledItemDelegate
{

    Q_OBJECT

    public:

    //* constructor
    TreeViewItemDelegate( QObject* parent = nullptr ):
        QStyledItemDelegate( parent )
    {}

    //* set item margin
    virtual void setItemMargin( int value )
    { itemMargin_ = value; }

    //* size hint for index
    virtual QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
    {
        QSize size( QStyledItemDelegate::sizeHint( option, index ) );
        if( size.isValid() && itemMargin_ > 0 )
        { size.rheight() += itemMargin_*2; }

        return size;
    }

    private:

    //* margin
    int itemMargin_ = 1;

};

#endif

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

#include "ItemModel.h"
#include "Debug.h"

//_______________________________________________________________
ItemModel::ItemModel( QObject* parent ):
    QAbstractItemModel( parent ),
    sortColumn_(0),
    sortOrder_( Qt::AscendingOrder )
{ Debug::Throw( QStringLiteral("ItemModel::ItemModel.\n") ); }

//____________________________________________________________
void ItemModel::sort( int column, Qt::SortOrder order )
{
    Debug::Throw() << "ItemModel::sort - column: " << column << " order: " << order << Qt::endl;

    // store column and order
    sortColumn_ = column;
    sortOrder_ = order;

    // emit signals and call private methods
    emit layoutAboutToBeChanged();
    _sort( column, order );
    emit layoutChanged();

}

//____________________________________________________________
QModelIndexList ItemModel::indexes( int column, const QModelIndex& parent ) const
{
    QModelIndexList out;
    int rows( rowCount( parent ) );
    for( int row = 0; row < rows; row++ )
    {
        auto index( this->index( row, column, parent ) );
        if( !index.isValid() ) continue;
        out.append( index );
        out.append( indexes( column, index ) );
    }

    return out;

}

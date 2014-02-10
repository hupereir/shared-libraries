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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"
#include "SpellItemModel.h"
#include "SpellItemModel.moc"

namespace SpellCheck
{

    //_______________________________________________
    const QString SpellItemModel::columnTitles_[ SpellItemModel::nColumns ] =
    { tr( "Item" ) };

    //__________________________________________________________________
    QVariant SpellItemModel::data( const QModelIndex& index, int role ) const
    {
        Debug::Throw( "SpellItemModel::data.\n" );


        // check index, role and column
        if( !index.isValid() ) return QVariant();

        // retrieve associated file info
        const QString& text( get(index) );

        switch( index.column() )
        {

            case Text:
            if( role == Qt::DisplayRole ) return text;
            else if( role == Qt::CheckStateRole ) return disabledItems_.contains( text ) ?  Qt::Unchecked : Qt::Checked;
            else return QVariant();

            default: break;

        }

        return QVariant();

    }

    //__________________________________________________________________
    QVariant SpellItemModel::headerData(int section, Qt::Orientation orientation, int role) const
    {

        if(
            orientation == Qt::Horizontal &&
            role == Qt::DisplayRole &&
            section >= 0 &&
            section < nColumns )
        { return columnTitles_[section]; }

        // return empty
        return QVariant();

    }

}

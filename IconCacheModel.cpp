
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

#include "IconCacheModel.h"

//_______________________________________________
const QString IconCacheModel::columnTitles_[ IconCacheModel::nColumns ] =
{
    tr( "Icon" ),
    tr( "Files" ),
    tr( "Sizes" )
};

//__________________________________________________________________
QVariant IconCacheModel::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // associated icon pair
    const BASE::IconCache::Pair& iconPair( get(index) );

    if( role == Qt::DisplayRole )
    {
        switch( index.column() )
        {

            case Icon: return iconPair.first;
            case Files: return iconPair.second.files().join( "\n" );
            case Sizes:
            if( !iconPair.second.isNull() )
            {
                QString buffer;
                QTextStream what( &buffer );
                bool first( true );
                foreach( const QSize& size, iconPair.second.availableSizes() )
                {
                    if( first ) first = false;
                    else what << ", ";
                    what << size.width() << "x" << size.height();
                }

                return buffer;

            }

            default: break;
        }

    } else if( role == Qt::DecorationRole && index.column() == Icon ) {

        return iconPair.second;

    }

    return QVariant();

}

//__________________________________________________________________
QVariant IconCacheModel::headerData(int section, Qt::Orientation orientation, int role) const
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

//________________________________________________________
bool IconCacheModel::SortFTor::operator () ( BASE::IconCache::Pair first, BASE::IconCache::Pair second ) const
{

    if( order_ == Qt::DescendingOrder ) std::swap( first, second );

    switch( type_ )
    {

        case Icon: return first.first < second.first;
        case Files: return first.second.files().join( "" ) < second.second.files().join( "" );
        default: return true;
    }

}

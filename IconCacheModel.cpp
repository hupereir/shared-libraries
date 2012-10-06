
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

#include "IconCacheModel.h"

//_______________________________________________
const QString IconCacheModel::columnTitles_[ IconCacheModel::nColumns ] =
{ "Icon", "Files" };

//__________________________________________________________________
QVariant IconCacheModel::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // associated icon pair
    const BASE::IconCache::Pair& iconPair( get(index) );

    switch( index.column() )
    {

        case Icon:
        // return text associated to file and column
        if( role == Qt::DisplayRole ) return iconPair.first;
        else if( role == Qt::DecorationRole ) return iconPair.second;
        break;

        case Files:
        if( role == Qt::DisplayRole ) return iconPair.second.files().join( "\n" );
        break;

        default: break;

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

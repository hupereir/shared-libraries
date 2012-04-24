
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

#include "PixmapCacheModel.h"

//_______________________________________________
const QString PixmapCacheModel::columnTitles_[ PixmapCacheModel::nColumns ] =
{ "Icon" };

//__________________________________________________________________
QVariant PixmapCacheModel::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // retrieve associated file info
    const PixmapEngine::Pair& icon_pair( get(index) );

    // return text associated to file and column
    if( role == Qt::DisplayRole && index.column() == Icon ) return icon_pair.first;
    if( role == Qt::DecorationRole && index.column() == Icon ) return icon_pair.second;
    return QVariant();

}

//__________________________________________________________________
QVariant PixmapCacheModel::headerData(int section, Qt::Orientation orientation, int role) const
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

//____________________________________________________________
void PixmapCacheModel::_sort( int column, Qt::SortOrder order )
{

    Debug::Throw() << "PixmapCacheModel::sort - column: " << column << " order: " << order << endl;
    std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) );

}

//________________________________________________________
bool PixmapCacheModel::SortFTor::operator () ( PixmapEngine::Pair first, PixmapEngine::Pair second ) const
{

    if( order_ == Qt::DescendingOrder ) std::swap( first, second );

    switch( type_ )
    {

        case Icon: return first.first < second.first;
        default: return true;
    }

}

#ifndef _IconCacheModel_h_
#define _IconCacheModel_h_

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

#include "Counter.h"
#include "Debug.h"
#include "IconCache.h"
#include "ListModel.h"

//! qlistview for object IconCaches
class IconCacheModel: public ListModel<BASE::IconCache::Pair>, public Counter
{

    public:

    //! number of columns
    enum { nColumns = 3 };

    //! column type enumeration
    enum ColumnType { Icon, Files, Sizes };

    //! constructor
    IconCacheModel( QObject* parent = 0 ):
        ListModel<BASE::IconCache::Pair>( parent ),
        Counter( "IconCacheModel" )
    {}

    //!@name methods reimplemented from base class
    //@{

    // return data for a given index
    virtual QVariant data(const QModelIndex &index, int role) const;

    //! header data
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //! number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return nColumns; }

    //@}

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    { std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) ); }

    private:

    //! used to sort IconCaches
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //! constructor
        SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
            ItemModel::SortFTor( type, order )
        {}

        //! prediction
        bool operator() ( BASE::IconCache::Pair, BASE::IconCache::Pair ) const;

    };

    //! column titles
    static const QString columnTitles_[ nColumns ];

};

#endif

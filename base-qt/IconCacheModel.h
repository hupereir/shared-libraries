#ifndef IconCacheModel_h
#define IconCacheModel_h

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
#include "Debug.h"
#include "IconCache.h"
#include "ListModel.h"

//* qlistview for object IconCaches
class IconCacheModel: public ListModel<Base::IconCache::Pair>, private Base::Counter<IconCacheModel>
{

    Q_OBJECT

    public:

    //* column type enumeration
    enum ColumnType
    {
        Icon,
        Files,
        Sizes,
        nColumns
    };

    //* constructor
    IconCacheModel( QObject* parent = 0 ):
        ListModel<Base::IconCache::Pair>( parent ),
        Counter( "IconCacheModel" )
    {}

    //*@name methods reimplemented from base class
    //@{

    // return data for a given index
    virtual QVariant data(const QModelIndex &index, int role) const;

    //* header data
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //* number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return nColumns; }

    //@}

    protected:

    //* sort
    void _sort( int column, Qt::SortOrder order ) override
    { std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) ); }

    //* get sizes from icon pair
    static QString _availableSizes( const Base::IconCache::Pair& );

    private:

    //* used to sort IconCaches
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        SortFTor( int type, Qt::SortOrder order ):
            ItemModel::SortFTor( type, order )
        {}

        //* prediction
        bool operator() ( Base::IconCache::Pair, Base::IconCache::Pair ) const;

    };

    //* column titles
    static const QString columnTitles_[ nColumns ];

};

#endif

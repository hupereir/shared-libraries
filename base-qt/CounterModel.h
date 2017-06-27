#ifndef CounterModel_h
#define CounterModel_h

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

#include "ListModel.h"
#include "CounterMap.h"
#include "Debug.h"

//* counter pair
class CounterPair: public Base::CounterMap::Pair
{

    public:

    //* constructor
    explicit CounterPair()
    {}

    //* constructor
    explicit CounterPair( const Base::CounterMap::Pair& other ):
        Base::CounterMap::Pair( other )
    {}

    //* less than operator
    bool operator < ( const CounterPair& pair ) const
    { return first < pair.first; }

    //* equal to operator
    bool operator == (const CounterPair& pair ) const
    { return first == pair.first; }

};

//* qlistview for object counters
class CounterModel: public ListModel<CounterPair>
{

    Q_OBJECT

    public:

    //* column type enumeration
    enum ColumnType
    {
        Name,
        Count,
        nColumns
    };

    //*@name methods reimplemented from base class
    //@{

    // return data for a given index
    QVariant data(const QModelIndex &index, int role) const override;

    //* header data
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    //* number of columns for a given index
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    { return nColumns; }

    //@}

    protected:

    //* sort
    void _sort( int, Qt::SortOrder ) override;

    private:

    //* used to sort IconCaches
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        explicit SortFTor( int type, Qt::SortOrder order ):
            ItemModel::SortFTor( type, order )
            {}

        //* prediction
        bool operator() ( CounterPair, CounterPair ) const;

    };

    //* column titles
    const std::array<QString, nColumns> columnTitles_ =
    {{
        tr( "Name" ),
        tr( "Counts" )
    }};

};

#endif

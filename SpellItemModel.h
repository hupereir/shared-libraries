#ifndef SpellItemModel_h
#define SpellItemModel_h
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

#include "Counter.h"
#include "ListModel.h"

#include <QSet>
#include <QString>

namespace SpellCheck
{

    class SpellItemModel: public ListModel<QString>, public Counter
    {

        Q_OBJECT

        public:

        //! number of columns
        enum { nColumns = 1 };

        //! constructor
        SpellItemModel( QObject* parent = 0 ):
            ListModel<QString>( parent ),
            Counter( "SpellItemModel" )
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

        //! items
        using ItemSet = QSet<QString>;

        //! set disabled items
        void setDisabledItems( const ItemSet& items )
        {
            emit layoutAboutToBeChanged();
            disabledItems_ = items;
            emit layoutChanged();
        }

        //! disabled items
        ItemSet disabledItems( void ) const
        { return disabledItems_; }

        protected:

        //! sorting
        virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
        {}

        private:

        //! column titles
        static const QString columnTitles_[ nColumns ];

        //! disabled items
        ItemSet disabledItems_;

    };

}

#endif

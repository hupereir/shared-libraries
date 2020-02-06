#ifndef ItemModel_h
#define ItemModel_h

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

#include "base_qt_export.h"
#include <QAbstractItemModel>
#include <QColor>

//* Job model. Stores job information for display in lists
class BASE_QT_EXPORT ItemModel : public QAbstractItemModel
{

    Q_OBJECT

    public:

    //* constructor
    explicit ItemModel(QObject* = nullptr);

    //*@name accessors
    //@{

    //* return all indexes in model starting from parent [recursive]
    QModelIndexList indexes( int column = 0, const QModelIndex& parent = QModelIndex() ) const;

    //* current sorting column
    int sortColumn() const
    { return sortColumn_; }

    //* current sort order
    Qt::SortOrder sortOrder() const
    { return sortOrder_; }

    //* get list of internal selected items
    virtual QModelIndexList selectedIndexes() const = 0;

    //* restore currentIndex
    virtual QModelIndex currentIndex() const = 0;

    //* true if expended indexes are supported
    virtual bool supportsExpandedIndexes() const
    { return false; }

    //* get list of internal selected items
    virtual QModelIndexList expandedIndexes() const
    { return QModelIndexList(); }

    //@}

    //*@name modifiers
    //@{

    //* sort
    virtual void sort()
    { sort( sortColumn(), sortOrder() ); }

    //* sort
    void sort( int column, Qt::SortOrder order ) override;

    //* clear internal list selected items
    virtual void clearSelectedIndexes() = 0;

    //* set selected indexes
    virtual void setSelectedIndexes( const QModelIndexList& ) = 0;

    //* store index internal selection state
    virtual void setIndexSelected( const QModelIndex&, bool ) = 0;

    //* current index;
    virtual void clearCurrentIndex() = 0;

    //* store current index
    virtual void setCurrentIndex( const QModelIndex& ) = 0;

    //* clear internal list of expanded items
    virtual void clearExpandedIndexes()
    {}

    //* set selected indexes
    virtual void setExpandedIndexes( const QModelIndexList& )
    {}

    //* store index internal selection state
    virtual void setIndexExpanded( const QModelIndex&, bool )
    {}

    //@}

    protected:

    //* this sort columns without calling the layout changed callbacks
    virtual void _sort()
    { _sort( sortColumn(), sortOrder() ); }

    //* private sort, with no signals emitted
    virtual void _sort( int column, Qt::SortOrder order )
    {}

    //* used to sort items in list
    class BASE_QT_EXPORT SortFTor
    {

        public:

        //* constructor
        explicit SortFTor( int type, Qt::SortOrder order ):
            type_( type ),
            order_( order )
        {}

        //* destructor
        virtual ~SortFTor() {};

        protected:

        //* column
        int type_;

        //* order
        Qt::SortOrder order_;

    };

    private:

    //* sorting column
    int sortColumn_ = 0;

    //* sorting order
    Qt::SortOrder sortOrder_ = Qt::AscendingOrder;

};

#endif

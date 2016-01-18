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

#include <QAbstractItemModel>
#include <QColor>

//* Job model. Stores job information for display in lists
class ItemModel : public QAbstractItemModel
{

    Q_OBJECT

    public:

    //* constructor
    ItemModel(QObject *parent = nullptr);

    //* return all indexes in model starting from parent [recursive]
    QModelIndexList indexes( int column = 0, const QModelIndex& parent = QModelIndex() ) const;

    //*@name sorting
    //@{

    //* sort
    virtual void sort( void )
    { sort( sortColumn(), sortOrder() ); }

    //* sort
    virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );

    //* current sorting column
    const int& sortColumn( void ) const
    { return sortColumn_; }

    //* current sort order
    const Qt::SortOrder& sortOrder( void ) const
    { return sortOrder_; }

    //@}

    //*@name selected indexes
    //@{

    //* clear internal list selected items
    virtual void clearSelectedIndexes( void ) = 0;

    //* set selected indexes
    virtual void setSelectedIndexes( const QModelIndexList& ) = 0;

    //* store index internal selection state
    virtual void setIndexSelected( const QModelIndex&, bool ) = 0;

    //* get list of internal selected items
    virtual QModelIndexList selectedIndexes( void ) const = 0;

    //@}

    //*@name current index
    //@{

    //* current index;
    virtual void clearCurrentIndex( void ) = 0;

    //* store current index
    virtual void setCurrentIndex( const QModelIndex& ) = 0;

    //* restore currentIndex
    virtual QModelIndex currentIndex( void ) const = 0;

    //@}

    //*@name expanded indexes
    //@{

    //* true if expended indexes are supported
    virtual bool supportsExpandedIndexes( void ) const
    { return false; }

    //* clear internal list of expanded items
    virtual void clearExpandedIndexes( void )
    {}

    //* set selected indexes
    virtual void setExpandedIndexes( const QModelIndexList& )
    {}

    //* store index internal selection state
    virtual void setIndexExpanded( const QModelIndex&, bool )
    {}

    //* get list of internal selected items
    virtual QModelIndexList expandedIndexes( void ) const
    { return QModelIndexList(); }

    //@}

    protected:

    //* this sort columns without calling the layout changed callbacks
    virtual void _sort( void )
    { _sort( sortColumn(), sortOrder() ); }

    //* private sort, with no signals emmitted
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) = 0;

    //* used to sort items in list
    class SortFTor
    {

        public:

        //* constructor
        SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
            type_( type ),
            order_( order )
        {}

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

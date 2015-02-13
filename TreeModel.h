#ifndef TreeModel_h
#define TreeModel_h

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
#include "ItemModel.h"
#include "TreeItem.h"

#include <QList>

//* generic class to store structure in a model
template<class T> class TreeModel : public ItemModel
{

    public:

    //* value type
    using ValueType = T;

    //* reference
    using Reference = T&;

    //* reference
    using ConstReference = const T&;

    //* pointer
    using Pointer = T*;

    //* list of values
    using List = QList<ValueType>;

    //* iterator
    using ListIterator = QListIterator<ValueType>;

    //* item
    using Item = TreeItem<T>;

    //* constructor
    TreeModel(QObject *parent = 0):
        ItemModel( parent ),
        root_( map_ )
    {}

    //* destructor
    virtual ~TreeModel()
    {}

    //*@name methods reimplemented from base class
    //@{

    //* flags
    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if (!index.isValid()) return 0;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    //* unique index for given row, column and parent index
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
    {

        // check if index is valid
        if( !hasIndex( row, column, parent ) ) return QModelIndex();

        // fid parent item
        const Item& parentItem = parent.isValid() ? _find( parent.internalId() ) : root_;

        // return new index if matching child is found, or invalid index
        return (row < (int)parentItem.childCount()) ?
            createIndex( row, column, parentItem.child(row).id() ):
            QModelIndex();

    }

    //* index of parent
    virtual QModelIndex parent(const QModelIndex &index) const
    {

        // check index validity
        if (!index.isValid()) return QModelIndex();

        // retrieve associated job item
        const Item& childItem( _find(index.internalId() ) );

        // if no parent return invalid index
        if( !( childItem.hasParent() && childItem.parent().hasParent() ) ) return QModelIndex();

        // if parent is root return invalid index
        const Item& parentItem( childItem.parent() );
        const Item& grandParentItem( parentItem.parent() );

        // find parent position in list of grand parent
        int row(0);
        for( ; row < grandParentItem.childCount(); row++ )
        { if( &parentItem == &grandParentItem.child(row) ) return createIndex( row, 0, parentItem.id() ); }

        // not found
        return QModelIndex();

    }

    //* number of rows below given index
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {

        // check column
        if (parent.column() > 0) return 0;

        // store parent Job
        const Item& parentItem = parent.isValid() ? _find( parent.internalId() ) : root_;
        return parentItem.childCount();

    }

    //@}

    //*@name value to index matching
    //@{

    //* return index associated to a given value, starting from parent [recursive]
    virtual QModelIndex index( ConstReference value, const QModelIndex& parent = QModelIndex() ) const
    {

        // return parent index if job match
        if( parent.isValid() && _find( parent.internalId() ).get() == value ) return parent;

        // get rows associated to parent and loop over rows
        for( int row=0; row<rowCount( parent ); row++ )
        {
            // get child index
            QModelIndex found( index( value, index( row, 0, parent ) ) );
            if( found.isValid() ) return found;
        }

        // value not found. return invalid index
        return QModelIndex();

    }

    //* return all values [recursive]
    virtual List children( const QModelIndex& parent = QModelIndex() ) const
    {

        // retrieve parent item
        const Item& parentItem = parent.isValid() ? _find( parent.internalId() ) : root_;
        return parentItem.childValues();

    }

    //* return all values [recursive]
    virtual List children( ConstReference value ) const
    {

        const QModelIndex& index( this->index( value ) );
        return index.isValid() ? children( index ):List();
    }

    //* return value associated to given model index
    virtual ValueType get( const QModelIndex& index ) const
    {
        if( !index.isValid() ) return ValueType();
        else return _find( index.internalId() ).get();
    }

    //* return all values matching index list
    List get( const QModelIndexList& indexes ) const
    {
        List out;
        foreach( const QModelIndex& index, indexes )
        { if( index.isValid() ) out << get( index ); }
        return out;
    }

    //@}

    //*@name selection
    //@{

    //* clear internal list selected items
    virtual void clearSelectedIndexes( void )
    { selectedItems_.clear(); }

    //* set selected indexes
    virtual void setSelectedIndexes( const QModelIndexList& indexes )
    {
        selectedItems_.clear();
        foreach( const QModelIndex& index, indexes )
        { if( index.isValid() ) selectedItems_ << get( index ); }
    }

    //* store index internal selection state
    virtual void setIndexSelected( const QModelIndex& index, bool value )
    {
        if( !index.isValid() ) return;
        if( value ) selectedItems_ << get( index );
        else { selectedItems_.removeAll( get( index ) ); }
    }

    //* get list of internal selected items
    virtual QModelIndexList selectedIndexes( void ) const
    {
        QModelIndexList out;
        foreach( const ValueType& value, selectedItems_ )
        {
            QModelIndex index( this->index( value ) );
            if( index.isValid() ) out << index;
        }
        return out;
    }

    //@}

    //*@name current index
    //@{

    //* current index;
    virtual void clearCurrentIndex( void )
    { hasCurrentItem_ = false; }

    //* store current index
    virtual void setCurrentIndex( const QModelIndex& index )
    {
        if( index.isValid() )
        {

            hasCurrentItem_ = true;
            currentItem_ = get( index );

        } else hasCurrentItem_ = false;
    }

    //* restore currentIndex
    virtual QModelIndex currentIndex( void ) const
    { return hasCurrentItem_ ? this->index( currentItem_ ) : QModelIndex(); }

    //@}

    //*@name expansion
    //@{

    //* true if expended indexes are supported
    virtual bool supportsExpandedIndexes( void ) const
    { return true; }

    //* clear internal list of expanded items
    virtual void clearExpandedIndexes( void )
    { expandedItems_.clear(); }

    //* set selected indexes
    virtual void setExpandedIndexes( const QModelIndexList& indexes )
    {
        expandedItems_.clear();
        foreach( const QModelIndex& index, indexes )
        { if( index.isValid() ) expandedItems_ << get( index ); }
    }

    //* store index internal selection state
    virtual void setIndexExpanded( const QModelIndex& index, bool value )
    {
        if( !index.isValid() ) return;
        if( value ) expandedItems_ << get( index );
        else { expandedItems_.removeAll( get( index ) ); }
    }

    //* get list of internal selected items
    virtual QModelIndexList expandedIndexes( void ) const
    {
        QModelIndexList out;
        foreach( const ValueType& value, expandedItems_ )
        {
            QModelIndex index( this->index( value ) );
            if( index.isValid() ) out << index;
        }
        return out;
    }

    //@}

    //* add values
    void add( ConstReference value )
    { add( List() << value ); }

    //* add values
    void add( List values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        root_.update( values );
        _add( root_, values );
        _sort();
        emit layoutChanged();

        return;

    }

    //* replace
    bool replace( ConstReference first, ConstReference second )
    {
        Item* item( root_.find( first ) );
        if( item )
        {

            item->set( second );
            return true;

        } else return false;

    };

    //* update values
    /*!
    items that are not found in list are removed
    items that are found are updated
    */
    void set( List values )
    {

        // check if not empty
        if( values.empty() ) clear();
        else {

            emit layoutAboutToBeChanged();

            // sort values if requested
            if( sortValues_ )
            { std::sort( values.begin(), values.end() ); }

            root_.set( values );
            _add( root_, values );
            _sort();
            emit layoutChanged();
        }

        return;

    }

    //* update values
    /*!
    items that are not found in list are removed
    items that are found are updated
    */
    void set( ConstReference value, List values )
    {

        // find item matching value
        Item* item( root_.find( value ) );
        if( !item ) return;

        emit layoutAboutToBeChanged();
        if( values.empty() ) item->clear();
        else {

            // sort values if requested
            if( sortValues_ )
            { std::sort( values.begin(), values.end() ); }

            item->set( values );
            _add( *item, values );
            _sort();

        }

        emit layoutChanged();
    }

    //* root item
    const Item& root( void ) const
    { return root_; }

    //* remove
    virtual void remove( ConstReference value )
    { remove( List() << value ); }

    //* remove
    virtual void remove( List values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        _remove( root_, values );
        _resetTree();
        emit layoutChanged();
        return;

    }

    //* reset tree
    void resetTree( void )
    {
        emit layoutAboutToBeChanged();
        _resetTree();
        emit layoutChanged();
    }


    //* clear
    void clear( void )
    {

        emit layoutAboutToBeChanged();
        map_.clear();
        root_ = Item( map_ );
        emit layoutChanged();

    }

    //* sort values
    void setSortValues( bool value )
    { sortValues_ = value; }

    protected:

    //* root item
    Item& _root( void )
    { return root_; }

    //* add
    void _add( Item& item, List values )
    {
        foreach( ConstReference value, values )
        { item.add( value ); }
    }

    //* find item matching id
    const Item& _find( typename Item::Id id ) const
    {
        typename Item::Map::const_iterator iter( map_.find( id ) );
        return iter == map_.end() ? root_:*iter.value();
    }

    //* remove, without update
    void _remove( Item& parent, List& values )
    {

        // remove all values from selection
        foreach( const ValueType& value, values )
        { selectedItems_.removeAll( value ); }

        // remove children that are found in list, and remove from list
        for( int row = 0; row < parent.childCount(); )
        {
            int found( values.indexOf( parent.child(row).get() ) );
            if( found >= 0 )
            {
                parent.remove( row );
                values.removeAt( found );
            } else ++row;

        }

        // do the same starting from children, if there are remaining items to remove
        if( !values.isEmpty() )
        {
            for( int row = 0; row < parent.childCount(); ++row )
            { _remove( parent.child(row), values ); }
        }

    }

    //* reset tree
    /*! private version, with no signal emitted */
    void _resetTree( void )
    {
        List children( TreeModel::children() );
        map_.clear();
        root_ = Item( map_ );
        _add( root_, children );
        _sort();
    }

    private:

    //* item map
    /*! used to allow fast mapping between index and value */
    typename Item::Map map_;

    //* root item
    Item root_;

    //* selection
    List selectedItems_;

    //* expanded indexes
    List expandedItems_;

    //* true if values should be sorted when retrieved
    bool sortValues_ = true;

    //* true if current item is valid
    bool hasCurrentItem_ = false;

    //* current item
    ValueType currentItem_;

};

#endif

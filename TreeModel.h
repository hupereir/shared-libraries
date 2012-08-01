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

#include "Debug.h"
#include "ItemModel.h"
#include "TreeItem.h"

#include <QtCore/QList>

//! generic class to store structure in a model
template<class T> class TreeModel : public ItemModel
{

    public:

    //! value type
    typedef T ValueType;

    //! reference
    typedef T& Reference;

    //! reference
    typedef const T& ConstReference;

    //! pointer
    typedef T* Pointer;

    //! list of values
    typedef QList<ValueType> List;

    //! iterator
    typedef QListIterator<ValueType> ListIterator;

    //! item
    typedef TreeItem<T> Item;

    //! constructor
    TreeModel(QObject *parent = 0):
        ItemModel( parent ),
        map_( typename Item::Map() ),
        root_( map_ )
    {}

    //! destructor
    virtual ~TreeModel()
    {}

    //!@name methods reimplemented from base class
    //@{

    //! flags
    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        if (!index.isValid()) return 0;
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    //! unique index for given row, column and parent index
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

    //! index of parent
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
        const Item& grand_parentItem( parentItem.parent() );

        // find parent position in list of grand parent
        int row(0);
        for( ; row < int( grand_parentItem.childCount() ); row++ )
        { if( &parentItem == &grand_parentItem.child(row) ) return createIndex( row, 0, parentItem.id() ); }

        // not found
        return QModelIndex();

    }

    //! number of rows below given index
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    {

        // check column
        if (parent.column() > 0) return 0;

        // store parent Job
        const Item& parentItem = parent.isValid() ? _find( parent.internalId() ) : root_;
        return parentItem.childCount();

    }

    //@}

    //!@name value to index matching
    //@{

    //! return index associated to a given value, starting from parent [recursive]
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

    //! return all values [recursive]
    virtual List children( const QModelIndex& parent = QModelIndex() ) const
    {

        // retrieve parent item
        const Item& parentItem = parent.isValid() ? _find( parent.internalId() ) : root_;
        return parentItem.childValues();

    }

    //! return all values [recursive]
    virtual List children( ConstReference value ) const
    {

        const QModelIndex& index( this->index( value ) );
        return index.isValid() ? children( index ):List();
    }

    //! return value associated to given model index
    virtual ValueType get( const QModelIndex& index ) const
    {
        if( !index.isValid() ) return ValueType();
        else return _find( index.internalId() ).get();
    }

    //! return all values matching index list
    List get( const QModelIndexList& indexes ) const
    {
        List out;
        foreach( const QModelIndex& index, indexes )
        { if( index.isValid() ) out << get( index ); }
        return out;
    }

    //@}

    //!@name selection
    //@{

    //! clear internal list selected items
    void clearSelectedIndexes( void )
    {
        for( typename Item::Map::iterator iter = map_.begin(); iter != map_.end(); ++iter )
        { iter.value()->setFlag( Item::SELECTED, false ); }
    }

    //! store index internal selection state
    void setIndexSelected( const QModelIndex& index, bool value )
    {
        if( !index.isValid() ) return;
        typename Item::Map::iterator iter( map_.find( index.internalId() ) );
        if( iter != map_.end() ) iter.value()->setFlag( Item::SELECTED, value );
    }

    //! get list of internal selected items
    QModelIndexList selectedIndexes( void ) const
    {
        QModelIndexList out;
        for( typename Item::Map::const_iterator iter = map_.begin(); iter != map_.end(); ++iter )
        {

            if( !iter.value()->flag( Item::SELECTED ) ) continue;

            // retrieve and check index associated to job
            QModelIndex index( TreeModel::index( iter.value()->get() ) );
            if( index.isValid() ) out << index;

        }

        return out;
    }

    //@}

    //!@name expansion
    //@{

    //! clear internal list of expanded items
    void clearExpandedIndexes( void )
    {
        for( typename Item::Map::iterator iter = map_.begin(); iter != map_.end(); ++iter )
        { iter.value()->setFlag( Item::EXPANDED, false ); }
    }

    //! store index internal selection state
    void setIndexExpanded( const QModelIndex& index, bool value )
    {
        if( !index.isValid() ) return;
        typename Item::Map::iterator iter( map_.find( index.internalId() ) );
        if( iter != map_.end() ) iter.value()->setFlag( Item::EXPANDED, value );
    }

    //! get list of internal selected items
    QModelIndexList expandedIndexes( void ) const
    {
        QModelIndexList out;
        for( typename Item::Map::const_iterator iter = map_.begin(); iter != map_.end(); ++iter )
        {

            if( !iter.value()->flag( Item::EXPANDED ) ) continue;

            // retrieve and check index associated to job
            QModelIndex index( TreeModel::index( iter.value()->get() ) );
            if( index.isValid() ) out << index;

        }

        return out;
    }

    //@}

    //! add values
    void add( ConstReference value )
    { add( List() << value ); }

    //! add values
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

    //! update values
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
            if( map_.sortValues() )
            { std::sort( values.begin(), values.end() ); }

            root_.set( values );
            _add( root_, values );
            _sort();
            emit layoutChanged();
        }

        return;

    }

    //! update values
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
            if( map_.sortValues() )
            { std::sort( values.begin(), values.end() ); }

            item->set( values );
            _add( *item, values );
            _sort();

        }

        emit layoutChanged();
    }

    //! root item
    const Item& root( void ) const
    { return root_; }

    //! remove
    virtual void remove( ConstReference value )
    { remove( List() << value ); }

    //! remove
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

    //! reset tree
    void resetTree( void )
    {
        emit layoutAboutToBeChanged();
        _resetTree();
        emit layoutChanged();
    }


    //! clear
    void clear( void )
    {

        emit layoutAboutToBeChanged();
        map_.clear();
        root_ = Item( map_ );
        emit layoutChanged();

    }

    //! sort values
    void setSortValues( bool value )
    { map_.setSortValues( value ); }

    protected:

    //! root item
    Item& _root( void )
    { return root_; }

    //! add
    void _add( Item& item, List values )
    {
        foreach( ConstReference value, values )
        { item.add( value ); }
    }

    //! find item matching id
    const Item& _find( typename Item::Id id ) const
    {
        typename Item::Map::const_iterator iter( map_.find( id ) );
        return iter == map_.end() ? root_:*iter.value();
    }

    //! remove, without update
    void _remove( Item& parent, List& values )
    {

        // remove children that are found in list, and remove from list
        for( unsigned int row = 0; row < parent.childCount(); )
        {
            int found( values.indexOf( parent.child(row).get() ) );
            if( found >= 0 )
            {
                parent.remove( row );
                values.removeAt( found );
            } else row++;

        }

        // do the same starting from children, if there are remaining items to remove
        if( !values.isEmpty() )
        {
            for( unsigned int row = 0; row < parent.childCount(); row++ )
            { _remove( parent.child(row), values ); }
        }

    }

    //! reset tree
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

    //! item map
    /*! used to allow fast mapping between index and value */
    typename Item::Map map_;

    //! root item
    Item root_;

};

#endif

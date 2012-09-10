#ifndef ListModel_h
#define ListModel_h

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

#include "ItemModel.h"

#include <QtCore/QList>
#include <algorithm>

//! Job model. Stores job information for display in lists
template<typename T, typename EqualTo = std::equal_to<T>, typename LessThan = std::less<T> >
class ListModel : public ItemModel
{

    public:

    //! value type
    typedef T ValueType;

    //! reference
    typedef T& Reference;

    //! pointer
    typedef T* Pointer;

    //! value list
    typedef QList<ValueType> List;
    typedef QListIterator<ValueType> ListIterator;
    typedef QMutableListIterator<ValueType> MutableListIterator;

    //! constructor
    ListModel(QObject *parent = 0):
        ItemModel( parent )
    {}

    //! destructor
    virtual ~ListModel()
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

        // return invalid index if parent is valid
        if( parent.isValid() ) return QModelIndex();

        // check against values_
        return ( row < (int) values_.size() ) ? createIndex( row, column ):QModelIndex();

    }

    //! index of parent
    virtual QModelIndex parent(const QModelIndex &) const
    { return QModelIndex(); }

    //! number of rows below given index
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
    { return parent.isValid() ? 0:values_.size(); }

    //@}

    //!@name selection
    //@{

    //! clear internal list selected items
    virtual void clearSelectedIndexes( void )
    { selection_.clear(); }

    //! store index internal selection state
    virtual void setIndexSelected( const QModelIndex& index, bool value )
    {
        if( !index.isValid() ) return;
        if( value ) selection_ << get(index);
        else selection_.erase( std::remove_if( selection_.begin(), selection_.end(), std::bind2nd( EqualTo(), get(index) ) ), selection_.end() );
    }

    //! get list of internal selected items
    virtual QModelIndexList selectedIndexes( void ) const
    {

        QModelIndexList out;
        foreach( const ValueType& value, selection_ )
        {
            QModelIndex index( this->index( value ) );
            if( index.isValid() ) out << index;
        }
        return out;

    }

    //@}

    //!@name interface
    //@{

    //! add value
    virtual void add( const ValueType& value )
    {

        emit layoutAboutToBeChanged();
        _add( value );
        _sort();
        emit layoutChanged();

    }

    //! add values
    virtual void add( const List& values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        foreach( const ValueType& value, values ) _add( value );
        _sort();
        emit layoutChanged();

    }

    //! insert values
    virtual void insert( const QModelIndex& index, const ValueType& value )
    {
        emit layoutAboutToBeChanged();
        _insert( index, value );
        emit layoutChanged();
    }

    //! insert values
    virtual void insert( const QModelIndex& index, const List& values )
    {
        emit layoutAboutToBeChanged();

        // need to loop in reverse order so that the "values" ordering is preserved
        ListIterator iter( values );
        iter.toBack();
        while( iter.hasPrevious() )
        { _insert( index, iter.previous() ); }

        emit layoutChanged();
    }

    //! insert values
    virtual void replace( const QModelIndex& index, const ValueType& value )
    {
        if( !index.isValid() ) add( value );
        else {
            emit layoutAboutToBeChanged();
            setIndexSelected( index, false );
            values_[index.row()] = value;
            setIndexSelected( index, true );
            emit layoutChanged();
        }
    }

    //! remove
    virtual void remove( const ValueType& value )
    {

        emit layoutAboutToBeChanged();
        _remove( value );
        emit layoutChanged();
        return;

    }

    //! remove
    virtual void remove( const List& values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        foreach( const ValueType& value, values ) _remove( value );
        emit layoutChanged();
        return;

    }

    //! clear
    virtual void clear( void )
    { set( List() ); }

    //! update values from list
    /*!
    values that are not found in current are removed
    new values are set to the end.
    This is slower than the "set" method, but the selection is not cleared in the process
    */
    virtual void update( List values )
    {

        emit layoutAboutToBeChanged();

        // store values to be removed
        List removedValues;

        // update values that are common to both lists
        for( typename List::iterator iter = values_.begin(); iter != values_.end(); ++iter )
        {

            // see if iterator is in list
            typename List::iterator found_iter( std::find_if( values.begin(), values.end(), std::bind2nd( EqualTo(), *iter ) ) );
            if( found_iter == values.end() ) removedValues.push_back( *iter );
            else {
                *iter = *found_iter;
                values.erase( found_iter );
            }

        }

        // remove values that have not been found in new list
        foreach( const ValueType& value, removedValues ) _remove( value );

        // add remaining values
        foreach( const ValueType& value, values ) _add( value );

        _sort();
        emit layoutChanged();

    }

    //! set all values
    virtual void set( const List& values )
    {

        emit layoutAboutToBeChanged();
        values_ = values;
        _sort();
        emit layoutChanged();

        return;
    }

    //! return all values
    const List& get( void ) const
    { return values_; }

    //! return true if model contains given index
    virtual bool contains( const QModelIndex& index ) const
    { return (index.isValid() && index.row() < int(values_.size()) ); }

    //! return value for given index
    virtual ValueType get( const QModelIndex& index ) const
    { return (index.isValid() && index.row() < int(values_.size()) ) ? values_[index.row()]:ValueType(); }

    //! return all values
    List get( const QModelIndexList& indexes ) const
    {
        List out;
        foreach( const QModelIndex& index, indexes )
        { if( index.isValid() && index.row() < int(values_.size()) ) out << get( index ); }
        return out;
    }

    //! return index associated to a given value
    virtual QModelIndex index( const ValueType& value, int column = 0 ) const
    {
        for( int row=0; row<values_.size(); ++row )
        { if( EqualTo()(value, values_[row]) ) return index( row, column ); }
        return QModelIndex();
    }

    //@}

    protected:

    //! return all values
    List& _get( void )
    { return values_; }

    //! add, without update
    virtual void _add( const ValueType& value )
    {
        typename List::iterator iter = std::find_if( values_.begin(), values_.end(), std::bind2nd( EqualTo(), value ) );
        if( iter == values_.end() ) values_ << value;
        else *iter = value;
    }

    //! add, without update
    virtual void _insert( const QModelIndex& index, const ValueType& value )
    {
        if( !index.isValid() ) add( value );
        int row = 0;
        typename List::iterator iter( values_.begin() );
        for( ;iter != values_.end() && row != index.row(); ++iter, ++row ){}
        values_.insert( iter, value );
    }

    //! remove, without update
    virtual void _remove( const ValueType& value )
    {
        values_.erase( std::remove_if( values_.begin(), values_.end(), std::bind2nd( EqualTo(), value )), values_.end() );
        selection_.erase( std::remove_if( selection_.begin(), selection_.end(), std::bind2nd( EqualTo(), value ) ), selection_.end() );
    }

    private:

    //! values
    List values_;

    //! selection
    List selection_;

};

#endif

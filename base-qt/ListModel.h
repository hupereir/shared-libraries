#ifndef ListModel_h
#define ListModel_h

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

#include "ItemModel.h"

#include <QList>

#include <algorithm>

//* Job model. Stores job information for display in lists
template<typename T, typename EqualTo = std::equal_to<T>>
class ListModel : public ItemModel
{

    public:

    //* value type
    using ValueType = T;

    //* reference
    using Reference = T&;

    //* pointer
    using Pointer = T*;

    //* value list
    using List = QList<ValueType>;
    using ListIterator = QListIterator<ValueType>;
    using MutableListIterator = QMutableListIterator<ValueType>;

    //* constructor
    explicit ListModel(QObject* parent = nullptr ):
        ItemModel( parent )
    {}

    //*@name accessors
    //@{

    //* flags
    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if (!index.isValid()) Qt::ItemFlags();
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    //* unique index for given row, column and parent index
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {

        // check if index is valid
        if( !hasIndex( row, column, parent ) ) return QModelIndex();

        // return invalid index if parent is valid
        if( parent.isValid() ) return QModelIndex();

        // check against values_
        return ( row < (int) values_.size() ) ? createIndex( row, column ):QModelIndex();

    }

    //* index of parent
    QModelIndex parent(const QModelIndex &) const override
    { return QModelIndex(); }

    //* number of rows below given index
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    { return parent.isValid() ? 0:values_.size(); }

    //* true if empty
    bool empty() const
    { return values_.empty(); }

    //* get list of internal selected items
    QModelIndexList selectedIndexes() const override
    {

        QModelIndexList out;
        for( const auto& value:selectedItems_ )
        {
            QModelIndex index( this->index( value ) );
            if( index.isValid() ) out.append( index );
        }
        return out;

    }

    //* restore currentIndex
    QModelIndex currentIndex() const override
    { return hasCurrentItem_ ? this->index( currentItem_ ) : QModelIndex(); }

    //* return all values
    const List& get() const
    { return values_; }

    //* return true if model contains given index
    bool contains( const QModelIndex& index ) const
    { return index.isValid() && index.row() < values_.size(); }

    //* return value for given index
    const ValueType& get( const QModelIndex& index ) const
    { return (index.isValid() && index.row() < values_.size() ) ? values_[index.row()]:default_; }

    //* return all values
    List get( const QModelIndexList& indexes ) const
    {
        List out;
        for( const auto& index:indexes )
        { if( index.isValid() && index.row() < values_.size() ) out.append( get( index ) ); }
        return out;
    }

    //* return index associated to a given value
    QModelIndex index( const ValueType& value, int column = 0 ) const
    {
        for( int row=0; row<values_.size(); ++row )
        { if( EqualTo()(value, values_[row]) ) return index( row, column ); }
        return QModelIndex();
    }

    //@}

    //*@name modifiers
    //@{

    //* clear internal list selected items
    void clearSelectedIndexes() override
    { selectedItems_.clear(); }

    //* set selected indexes
    void setSelectedIndexes( const QModelIndexList& indexes ) override
    {
        selectedItems_.clear();
        for( const auto& index:indexes )
        { if( contains( index ) ) selectedItems_.append( values_[index.row()] ); }
    }

    //* store index internal selection state
    void setIndexSelected( const QModelIndex& index, bool value ) override
    {
        if( !contains( index ) ) return;
        if( value ) selectedItems_.append( values_[index.row()] );
        else selectedItems_.erase(
            std::remove_if( selectedItems_.begin(), selectedItems_.end(),
            [this, &index](const ValueType& value) { return EqualTo()( value, values_[index.row()] ); } ),
            selectedItems_.end() );
    }

    //* current index;
    void clearCurrentIndex() override
    { hasCurrentItem_ = false; }

    //* store current index
    void setCurrentIndex( const QModelIndex& index ) override
    {
        if( contains( index ) )
        {

            hasCurrentItem_ = true;
            currentItem_ = values_[index.row()];

        } else hasCurrentItem_ = false;
    }

    //* add value
    void add( const ValueType& value )
    {

        emit layoutAboutToBeChanged();
        _add( value );
        emit layoutChanged();

    }

    //* add values
    void add( const List& values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        _add( values );
        emit layoutChanged();

    }

    //* insert a value
    void insert( const QModelIndex& index, const ValueType& value )
    {
        emit layoutAboutToBeChanged();

        if( index.isValid() && index.row() < values_.size() ) _insert( index, value );
        else _add( value );

        emit layoutChanged();
    }

    //* insert values
    void insert( const QModelIndex& index, const List& values )
    {
        emit layoutAboutToBeChanged();

        if( index.isValid() && index.row() < values_.size() )
        {

            _insert( index, values );

        } else {

            _add( values );
            return;

        }

        emit layoutChanged();
    }

    //* replace a value
    void replace( const QModelIndex& index, const ValueType& value )
    {
        if( !contains( index ) ) add( value );
        else {

            emit layoutAboutToBeChanged();

            // store old value
            ValueType oldValue( values_[index.row()] );

            // update
            values_[index.row()] = value;

            // update selection
            std::replace_if( selectedItems_.begin(), selectedItems_.end(),
                [&oldValue](const ValueType& value) { return EqualTo()( value, oldValue ); },
                value );

            emit layoutChanged();
        }
    }

    //* remove
    void remove( const ValueType& value )
    {

        emit layoutAboutToBeChanged();
        _remove( value );
        emit layoutChanged();
        return;

    }

    //* remove
    void remove( const List& values )
    {

        // check if not empty
        // this avoids sending useless signals
        if( values.empty() ) return;

        emit layoutAboutToBeChanged();
        for( const auto& value:values ) _remove( value );
        emit layoutChanged();
        return;

    }

    //* clear
    void clear()
    { set( List() ); }

    //* update values from list
    /**
    values that are not found in current are removed
    new values are set to the end.
    This is slower than the "set" method, but the selection is not cleared in the process
    */
    void update( List values )
    {

        emit layoutAboutToBeChanged();

        // store values to be removed
        List removedValues;

        // update values that are common to both lists
        for( auto&& iter = values_.begin(); iter != values_.end(); ++iter )
        {

            // see if iterator is in list
            auto foundIter( std::find_if( values.begin(), values.end(), std::bind2nd( EqualTo(), *iter ) ) );
            if( foundIter == values.end() ) removedValues.append( *iter );
            else {
                *iter = *foundIter;
                values.erase( foundIter );
            }

        }

        // remove values that have not been found in new list
        for( const auto& value:removedValues ) _remove( value );

        // add remaining values
        _add( values );

        emit layoutChanged();

    }

    //* set all values
    void set( const List& values )
    {

        emit layoutAboutToBeChanged();
        values_ = values;
        _sort();
        emit layoutChanged();

        return;
    }

    //@}

    protected:

    //* return all values
    List& _get()
    { return values_; }

    //* add, without update
    virtual void _add( const ValueType& value )
    {
        auto iter = std::find_if( values_.begin(), values_.end(), std::bind2nd( EqualTo(), value ) );
        if( iter != values_.end() ) { *iter = value; }
        else values_.append( value );
    }

    //* add, without update
    virtual void _add( const List& values )
    {

        for( const auto& value: values )
        {
            auto iter = std::find_if( values_.begin(), values_.end(), std::bind2nd( EqualTo(), value ) );
            if( iter != values_.end() ) { *iter = value; }
            else values_.append( value );
        }

        _sort();
    }

    //* insert, without update
    virtual void _insert( const QModelIndex& index, const ValueType& value )
    {
        auto iter = values_.begin() + index.row();
        values_.insert( iter, value );
    }

    //* insert, without update
    virtual void _insert( const QModelIndex& index, const List& values )
    {
        auto iter = values_.begin() + index.row();
        for( const auto& value:values )
        { iter = values_.insert( iter, value ); }
    }

    //* remove, without update
    virtual void _remove( const ValueType& value )
    {
        values_.erase( std::remove_if( values_.begin(), values_.end(), std::bind2nd( EqualTo(), value )), values_.end() );
        selectedItems_.erase( std::remove_if( selectedItems_.begin(), selectedItems_.end(), std::bind2nd( EqualTo(), value ) ), selectedItems_.end() );
    }

    private:

    //* values
    List values_;

    //* selection
    List selectedItems_;

    //* true if current item is valid
    bool hasCurrentItem_ = false;

    //* default item
    ValueType default_;

    //* current item
    ValueType currentItem_;

};

#endif

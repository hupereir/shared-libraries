#ifndef TreeItem_h
#define TreeItem_h

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

#include "TreeItemBase.h"

#include <QHash>
#include <QVector>

#include <algorithm>

//* generic "isChild" method
/** can be specialized for objects to which one cannot add an "isChild" member function */

namespace Base
{
    template<class T>
    bool isChild( const T& first, const T& second )
    { return first.isChild( second ); }
}

//* used to wrap object T into tree structure
template<class T> class TreeItem: public TreeItemBase
{

    public:

    //* value type
    using ValueType = T;

    //* reference
    using Reference = T&;

    //* reference
    using ConstReference = const T&;

    //* list of references
    using ValueList = QVector<T>;

    //* pointer
    using Pointer = T*;

    //* list of vector
    using List = QVector<TreeItem>;

    //* map items to internal id
    using Map = QHash<int, TreeItem*>;

    //* empty constructor
    TreeItem():
        TreeItemBase(0)
    {}

    //* root constructor
    TreeItem( Map& itemMap ):
        TreeItemBase(0),
        map_( &itemMap )
    { (*map_)[id()] = this; }

    //* copy constructor
    TreeItem( const TreeItem& item ):
        TreeItemBase( item.id() ),
        map_( item.map_ ),
        parent_( item.parent_ ),
        value_( item.value_ ),
        children_( item.children_ )
    {

        // store id in map
        (*map_)[id()] = this;

        // reassign parents
        for( auto& child:children_ ) { child.parent_ = this; }

    }

    //* assignment
    TreeItem& operator = ( const TreeItem& item )
    {

        // check for identity
        if( this == &item ) return *this;

        parent_ = item.parent_;
        value_ = item.value_;
        children_ = item.children_;
        map_ = item.map_;

        // erase current id from map
        // update and store in map
        _eraseFromMap();
        _setId( item.id() );
        (*map_)[id()] = this;

        // reassign parents
        for( auto& child:children_ ) { child.parent_ = this; }

        return *this;
    }

    //* destructor
    ~TreeItem() override
    { _eraseFromMap(); }

    //*@name accessors
    //@{

    //* value
    const ValueType& get() const
    { return value_; }

    //* parent
    bool hasParent() const
    { return parent_; }

    //* parent
    const TreeItem& parent() const
    { return *parent_; }

    //* child count
    int childCount() const
    { return children_.size(); }

    //* get child at given row
    const TreeItem& child( int row ) const
    { return children_[row]; }

    //* retrieve all children in list [recursive]
    ValueList childValues() const
    {

        ValueList out;
        for( const auto& item:children_ )
        {
            out.append( item.get() );
            out.append( item.childValues() );
        }

        return out;

    }

    //@}

    //*@name modifiers
    //@{

    //* clear children
    void clear()
    { children_.clear(); }

    //* value
    ValueType& get()
    { return value_; }

    //* value
    void set( ConstReference value )
    { value_ = value; }

    //* get child at given row
    TreeItem& child( int row )
    { return children_[row]; }

    //* find item matching value
    TreeItem* find( const ValueType& value )
    {

        // check against current value
        if( value == get() ) return this;

        // check against children
        for( auto& child:children_ )
        { if( TreeItem* out = child.find( value ) ) return out; }

        // not found. return null_ptr
        return nullptr;

    }

    //* find item matching value
    template<class Pred>
    TreeItem* findIf( const Pred& predicate )
    {
        if( predicate( get() ) ) return this;

        // check against children
        for( auto& child:children_ )
        { if( TreeItem* out = child.findIf( predicate ) ) return out; }

        return nullptr;
    }


    //* remove child at given row
    bool remove( int row )
    {
        if( row >= children_.size() ) return false;
        children_.erase( children_.begin() + row );
        return true;
    }

    //* add child (recursive)
    /** note: this code assumes that the value is not already in the tree */
    bool add( ConstReference value )
    {

        // try add to this list of children
        if( Base::isChild( value, get() ) )
        {
            children_.append( TreeItem( *map_, this, value ) );
            return true;
        }

        // try add to children
        bool added(false);
        for( auto& child:children_ )
        { if( child.add( value ) ) { added = true; break; } }

        // add to this if top level
        if( !( added || hasParent() ) )
        {
            children_.append( TreeItem( *map_, this, value ) );
            return true;
        }

        return added;

    }

    //* update children from existing values [recursive]
    /**
    updated items are removed from the set.
    children that are not found in the set are removed from the tree
    */
    void set( ValueList& values )
    {

        // check if there are remainig values
        if( values.isEmpty() )
        {

            children_.clear();
            return;

        }

        // update children that are found in set
        // remove children that are not found
        for( auto iter = children_.begin(); iter != children_.end(); )
        {
            const int found( values.indexOf( iter->get() ) );
            if( found < 0 ) iter = children_.erase( iter );
            else if( Base::isChild( values.at(found), get() ) )
            {

                // update child
                iter->set( values.at(found) );
                values.removeAt( found );

                // update child children recursively
                iter->set( values );
                ++iter;

            } else {

                // remove row and stop here
                iter = children_.erase( iter );

            }

        }

        return;

    }

    //* update children from existing values [recursive]
    /**
    updated items are removed from the set.
    children that are not found in the set are left unchanged
    */
    void update( ValueList& values )
    {

        // check if there are remainig values
        if( values.isEmpty() ) return;

        for( auto&& iter = children_.begin(); iter != children_.end(); )
        {

            const int found( values.indexOf( iter->get() ) );
            if( found >= 0 )
            {

                // make sure parent has not changed
                if( Base::isChild( values.at(found), get() ) )
                {

                    // re-assign and remove from list
                    iter->set( values.at(found) );
                    values.removeAt( found );

                } else {

                    // remove row and stop here
                    iter = children_.erase( iter );
                    continue;

                }

            }

            // update child children recursively
            iter->update( values );
            ++iter;

        }

        return;

    }

    //* sorting (recursive)
    void sort()
    {
        if( children_.empty() ) return;
        std::sort( children_.begin(), children_.end() );

        // do the same with children
        for( auto& child:children_ )
        { child.sort(); }

    }

    //* templatized sorting
    template<class SortMethod>
        void sort( SortMethod method )
    {

        if( children_.empty() ) return;
        std::sort( children_.begin(), children_.end(), method );

        // do the same with children
        for( auto& child:children_ )
        { child.sort( method ); }

    }

    //@}

    protected:

    //* constructor
    /** used to insert T in the tree structure */
    TreeItem( Map& itemMap, const TreeItem* parent, ConstReference value ):
        TreeItemBase( ++_runningId() ),
        map_( &itemMap ),
        parent_( parent ),
        value_( value )
    { (*map_)[id()] = this; }

    //* value
    Reference _get()
    { return value_; }

    //* erase from map
    void _eraseFromMap()
    {
        auto iter( map_->find( id() ) );
        if( iter != map_->end() && iter.value() == this )
        { map_->erase( iter ); }
    }

    private:

    //* item map
    Map* map_ = nullptr;

    //* parent
    const TreeItem* parent_ = nullptr;

    //* associated value
    ValueType value_;

    //* list of children
    List children_;

    //* streamer
    friend QTextStream& operator << (QTextStream& out , const TreeItem& item )
    {

        // indent 2 space characters per parent
        const auto parent( item.parent_ );
        while( parent )
        {
            out << "  ";
            parent = parent->parent_;
        }

        // print job pid
        out << item.id() << " " << item.get() << Qt::endl;

        // print job children
        for( const auto& child:item.children_ )
        { out << child; }

        return out;

    }

};

//* less than operator
template<class T>
inline bool operator < (const TreeItem<T>& lhs, const TreeItem<T>& rhs)
{ return lhs.get() < rhs.get(); }

//* equal to operator
template<class T>
inline bool operator == (const TreeItem<T>& lhs, const TreeItem<T>& rhs)
{ return lhs.get() == rhs.get(); }

#endif

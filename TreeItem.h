#ifndef TreeItem_h
#define TreeItem_h

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

/*!
\file    TreeItem.h
\brief   used to wrap object T into tree structure
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include "TreeItemBase.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <vector>

//! used to wrap object T into tree structure
template<class T> class TreeItem: public TreeItemBase
{

    public:

    //! value type
    typedef T ValueType;

    //! reference
    typedef T& Reference;

    //! reference
    typedef const T& ConstReference;

    //! set of references
    typedef std::set<T> ValueSet;

    //! list of references
    typedef std::vector<T> ValueList;

    //! pointer
    typedef T* Pointer;

    //! list of vector
    typedef std::vector<TreeItem> List;

    //! list of vector
    typedef std::map<int, TreeItem*> Map;

    //! id type
    typedef unsigned int Id;

    //! root constructor
    TreeItem( Map& itemMap ):
        TreeItemBase(0),
        map_( itemMap ),
        parent_(0)
    { map_[id()] = this; }

    //! copy constructor
    TreeItem( const TreeItem& item ):
        TreeItemBase( item.id() ),
        map_( item.map_ ),
        parent_( item.parent_ ),
        value_( item.value_ ),
        children_( item.children_ )
    {

        // flags
        setFlags( item.flags() );

        // store id in map
        map_[id()] = this;

        // reassign parents
        for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
        { iter->parent_ = this; }

    }

    //! assignment
    TreeItem& operator = ( const TreeItem& item )
    {

        // flags
        setFlags( item.flags() );

        parent_ = item.parent_;
        value_ = item.value_;
        children_ = item.children_;

        // erase current id from map
        // update and store in map
        _eraseFromMap();
        _setId( item.id() );
        map_[id()] = this;

        // reassign parents
        for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
        { iter->parent_ = this; }

        return *this;
    }

    //! destructor
    virtual ~TreeItem( void )
    { _eraseFromMap(); }

    //! less than operator
    bool operator < (const TreeItem& item ) const
    { return get() < item.get(); }

    //! equal to operator
    bool operator == (const TreeItem& item ) const
    { return get() == item.get(); }

    //! clear children
    void clear( void )
    { children_.clear(); }

    //! value
    const ValueType& get( void ) const
    { return value_; }

    //! value
    void set( ConstReference value )
    { value_ = value; }

    //! parent
    bool hasParent( void ) const
    { return parent_; }

    //! parent
    const TreeItem& parent( void ) const
    {
        assert( parent_ );
        return *parent_;
    }

    //! child count
    unsigned int childCount( void ) const
    { return children_.size(); }

    //! get child at given row
    TreeItem& child( unsigned int row )
    {
        assert( row < childCount() );
        return children_[row];
    }

    //! get child at given row
    const TreeItem& child( unsigned int row ) const
    {
        assert( row < childCount() );
        return children_[row];
    }

    //! retrieve all children in list [recursive]
    ValueList childValues( void ) const
    {

        ValueList out;
        for( typename List::const_iterator iter = children_.begin(); iter != children_.end(); iter++ )
        {
            out.push_back( iter->get() );
            ValueList children( iter->childValues() );
            out.insert( out.end(), children.begin(), children.end() );
        }

        return out;

    }

    //! remove child at given row
    bool remove( unsigned int row )
    {
        if( row >= childCount() ) return false;
        unsigned int local(0);
        typename List::iterator iter = children_.begin();
        for( ; iter != children_.end() && local < row; iter++, local++ ) {}
        children_.erase( iter );
        return true;
    }

    //! add child [recursive]
    /*! note: this code assumes that the value is not already in the tree */
    bool add( ConstReference value )
    {

        // try add to this list of children
        if( value.isChild( get() ) )
        {
            children_.push_back( TreeItem( map_, this, value ) );
            return true;
        }

        // try add to children
        bool added(false);
        for( typename List::iterator iter = children_.begin(); iter != children_.end() && !added; iter++ )
        { added = iter->add( value ); }

        // add to this if top level
        if( !( added || hasParent() ) )
        {
            children_.push_back( TreeItem( map_, this, value ) );
            return true;
        }

        return added;

    }

    //! update children from existing values [recursive]
    /*!
    updated items are removed from the set.
    children that are not found in the set are removed from the tree
    */
    void set( ValueSet& values )
    {

        // check if there are remainig values
        if( values.empty() ) return;

        // first update children that are found in set
        for( unsigned int row = 0; row < childCount(); )
        {
            typename ValueSet::iterator found( values.find( child(row).get() ) );
            if( found == values.end() ) remove( row );
            else {

                // update child
                child(row).set( *found );
                values.erase( found );

                // update child children recursively
                child(row).set( values );
                row++;

            }

        }

        return;

    }

    //! update children from existing values [recursive]
    /*!
    updated items are removed from the set.
    children that are not found in the set are left unchanged
    */
    void update( ValueSet& values )
    {

        // check if there are remainig values
        if( values.empty() ) return;

        // first update children that are found in set
        for( unsigned int row = 0; row < childCount(); row++ )
        {

            typename ValueSet::iterator found( values.find( child(row).get() ) );
            if( found != values.end() )
            {

                // make sure parent has not changed
                if( found->isChild( get() ) )
                {

                    // re-assign and remove from list
                    child(row).set( *found );
                    values.erase( found );

                } else {

                    // remove row and stop here
                    remove( row );
                    continue;

                }

            }

            child(row).update( values );

        }

        return;

    }

    //! sorting (recursive)
    void sort( void )
    {
        if( children_.empty() ) return;
        std::sort( children_.begin(), children_.end() );

        // do the same with children
        for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
        { iter->sort(); }

    }

    //! templatized sorting
    template<class SortMethod>
        void sort( SortMethod method )
    {

        if( children_.empty() ) return;
        std::sort( children_.begin(), children_.end(), method );

        // do the same with children
        for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
        { iter->sort( method ); }

    }

    protected:

    //! constructor
    /*! used to insert T in the tree structure */
    TreeItem( Map& itemMap, const TreeItem* parent, ConstReference value ):
        TreeItemBase( ++_runningId() ),
        map_( itemMap ),
        parent_( parent ),
        value_( value )
    { map_[id()] = this; }

    //! value
    Reference _get( void )
    { return value_; }

    //! value
    void _set( ConstReference value )
    { value_ = value; }

    //! erase from map
    void _eraseFromMap( void )
    {
        typename Map::iterator iter( map_.find( id() ) );
        if( iter != map_.end() && iter->second == this ) map_.erase( id() );

    }

    private:

    //! item map
    Map& map_;

    //! parent
    const TreeItem* parent_;

    //! associated value
    ValueType value_;

    //! list of children
    List children_;

    //! streamer
    friend QTextStream& operator << (QTextStream& out , const TreeItem& item )
    {

        // indent 2 space characters per parent
        const TreeItem* parent( item.parent_ );
        while( parent )
        {
            out << "  ";
            parent = parent->parent_;
        }

        // print job pid
        out << item.id() << " " << item.get() << endl;

        // print job children
        for( unsigned int i=0; i<item.childCount(); i++ )
        { out << item.child(i); }

        return out;

    }

};

#endif

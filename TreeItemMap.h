#ifndef TreeItemMap_h
#define TreeItemMap_h

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

#include <cassert>
#include <vector>
#include <list>

//! used to wrap object T into tree structure
template<class T> class TreeItemMap
{

    public:

    //! value type
    typedef T ValueType;

    //! reference
    typedef T& Reference;

    //! pointer
    typedef T* Pointer;

    //! insert item in map
    unsigned int insert( Reference item )
    {

        // check if there are available slots
        // if not, add to the end of the vector
        if( slots_.empty() )
        {
            items_.push_back( &item );
            return items_.size() - 1;
        } else {
            double out = slots_.front();
            assert( !items_[out] );
            items_[out] = &item;
            slots_.pop_front();
            return out;
        }
    }

    //! remove item at given slot
    void erase( const unsigned int& index )
    {
        assert( index < items_.size() );
        assert( items_[index] );
        items_[index] = 0;
        slots_.push_back( index );
    }

    //! array size
    unsigned int size( void ) const
    { return items_.size(); }

    //! true if index is valid
    bool isValid( const unsigned int& index ) const
    { return index < items_.size() && items_[index]; }

    //! get item at given index
    Reference get( const unsigned int& index ) const
    {
        assert( index < items_.size() );
        assert( items_[index] );
        return *items_[index];
    }

    //! clear maps
    void clear( void )
    {
        items_.clear();
        slots_.clear();
    }

    private:

    //! vector of stored items
    std::vector<T*> items_;

    //! list of empty slots
    std::list<unsigned int> slots_;

};

#endif

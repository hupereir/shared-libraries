#ifndef CppUtil_h
#define CppUtil_h

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

#include "IntegralType.h"

#include <QHashFunctions>
#include <algorithm>

namespace Base
{

    //* copy one container to another
    template<class T, class U>
        T makeT( const U& in )
    {
        T out;
        std::copy( in.begin(), in.end(), std::back_inserter( out ) );
        return out;
    }

    //* equivalent-to pseudo-operator
    /** it is used for smart insertion in maps and hashed */
    template<class T, class U>
        constexpr inline bool areEquivalent(const T& first, const U& second)
    { return !(first < second || second < first); }

    //* efficient map insertion
    template<class T>
        inline typename T::iterator insert( T& map, const typename T::key_type& key, const typename T::mapped_type& value )
    {
        auto iterator = map.lowerBound( key );
        if( iterator != map.end() && areEquivalent( key, iterator.key() ) )
        {
            iterator.value() = value;
            return iterator;
        } else {
            return map.insert( iterator, key, value );
        }
    }

    //* find iterator by value in map
    template<class T>
        inline typename T::const_iterator findByValue( const T& map, const typename T::mapped_type& value )
    { return std::find_if( map.begin(), map.end(), [&value](const typename T::mapped_type& current) { return current == value; } ); }

    //* get the sign of a quantity
    template <typename T> int sign(T val)
    { return (T(0) < val) - (val < T(0)); }

}

//* fancy qhash for all enum types
template<class T,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
    inline uint qHash( const T& value )
{ return qHash(Base::toIntegralType(value)); }

#endif

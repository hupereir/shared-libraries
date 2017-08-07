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

#include <QtGlobal>

#if QT_VERSION >= 0x050100
#include <QHashFunctions>
#else
#include <QHash>
#endif

#include <initializer_list>
#include <utility>

namespace Base
{

    template<typename T>
        using underlying_type_t = typename std::underlying_type<T>::type;

    //* convert an strong type enum to integral type
    template<typename T>
    constexpr typename std::underlying_type<T>::type
    toIntegralType(T value) noexcept
    { return static_cast<underlying_type_t<T>>(value);}

    template<typename T>
    T makeT( std::initializer_list<std::pair<typename T::key_type, typename T::mapped_type> >&& reference )
    {
        #if QT_VERSION >= 0x050100
        return T( std::move( reference ) );
        #else
        // for old QT versions there is no container constructor from initializer_list
        T out;
        for( auto&& pair:std::move(reference) ) { out.insert( pair.first, pair.second ); }
        return out;
        #endif
    }

    template<typename T>
    T makeT( std::initializer_list<typename T::key_type>&& reference )
    {
        #if QT_VERSION >= 0x050100
        return T( std::move( reference ) );
        #else
        // for old QT versions there is no container constructor from initializer_list
        T out;
        for( auto&& value:std::move(reference) ) { out.insert( value ); }
        return out;
        #endif
    }

    //* append initializer_list to a container
    template<typename T>
    void append( T& first, std::initializer_list<typename T::value_type>&& second )
    {
        #if QT_VERSION >= 0x050500
        first.append( std::move( second ) );
        #else
        // for old QT versions there is no append to some containers from initializer_list
        for( const auto& content:std::move(second) )
        { first.append( content ); }
        #endif
    }


    //* efficient map insertion
    template<typename T>
    void insert( T& map, const typename T::key_type& key, const typename T::mapped_type& value )
    {
        auto iterator = map.lowerBound( key );
        if( iterator != map.end() && !( key < iterator.key() || iterator.key() < key ) )
        {

            iterator.value() = value;

        } else {

            map.insert( iterator, key, value );

        }

    }

}

//* fancy qhash for all enum types
template<typename T,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
    uint qHash( const T& value )
{ return qHash(Base::toIntegralType(value)); }

//* different from operator
template<class T, class U>
    bool operator != (const T& first, const U& second)
{ return !(first == second); }

//* more than
template<class T, class U>
    bool operator > (const T& first, const U& second)
{ return second < first; }

//* less or equal
template<class T, class U>
    bool operator <= (const T& first, const U& second)
{ return !(second < first); }

//* more or equal
template<class T, class U>
    bool operator >= (const T& first, const U& second)
{ return !(first < second); }

#endif

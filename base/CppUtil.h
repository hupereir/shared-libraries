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

#include <QList>
#include <QVector>

#include <algorithm>
#include <initializer_list>
#include <utility>

namespace Base
{

    //* converninece trait for underlying type
    template<class T>
        using underlying_type_t = typename std::underlying_type<T>::type;

    //* convert an strong type enum to integral type
    template<class T>
        constexpr underlying_type_t<T>
        toIntegralType(T value) noexcept
    { return static_cast<underlying_type_t<T>>(value);}

    //* construct QHash, QMap from initializer_list
    template<class T>
        inline T makeT( std::initializer_list<std::pair<typename T::key_type, typename T::mapped_type> >&& reference )
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

    //* construct QSet from initializer_list
    template<class T>
        inline T makeT( std::initializer_list<typename T::key_type>&& reference )
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

    //* construct QList or QVector from initializer_list
    template<
        class T,
        typename = typename std::enable_if<
            std::is_base_of<QList<typename T::value_type>, typename std::decay<T>::type>::value ||
            std::is_base_of<QVector<typename T::value_type>, typename std::decay<T>::type>::value
            >::type
        >
        inline T makeT( std::initializer_list<typename T::value_type>&& reference )
    {
        #if QT_VERSION >= 0x040800
        return T( std::move(reference) );
        #else
        T out;
        std::copy( reference.begin(), reference.end(), std::back_inserter(out) );
        return out;
        #endif
    }

    //* append initializer_list to a container
    template<class T>
        inline void append( T& first, std::initializer_list<typename T::value_type>&& second )
    {
        #if QT_VERSION >= 0x050500
        first.append( std::move( second ) );
        #else
        std::copy( second.begin(), second.end(), std::back_inserter(first) );
        #endif
    }

    //* equivalent-to pseudo-operator
    /** it is used for smart insertion in maps and hashed */
    template<class T, class U>
        inline bool areEquivalent(const T& first, const U& second)
    { return !(first < second || second < first); }

    //* efficient map insertion
    template<class T>
        inline typename T::iterator insert(
        T& map,
        const typename T::const_iterator iterator,
        const typename T::key_type& key,
        const typename T::mapped_type& value )
    {

        #if QT_VERSION >= 0x050100
        return map.insert( iterator, key, value );
        #else
        Q_UNUSED( iterator );
        return map.insert( key, value );
        #endif

    }

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

            return insert( map, iterator, key, value );

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

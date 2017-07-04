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

    #if QT_VERSION >= 0x050100
    // for new Qt versoins, QHash can be constructed from initializer_list. So just move the arguments
    template<typename Key, typename T>
    std::initializer_list<std::pair<Key,T>> makeHash( std::initializer_list<std::pair<Key,T>>&& reference )
    { return std::move( reference ); }

    // for new Qt versoins, QMap can be constructed from initializer_list. So just move the arguments
    template<typename Key, typename T>
    std::initializer_list<std::pair<Key,T>> makeMap( std::initializer_list<std::pair<Key,T>>&& reference )
    { return std::move( reference ); }

    #else
    // for old QT versions there is no QHash constructor from initializer_list
    template<typename Key, typename T>
    QHash<Key,T> makeHash( std::initializer_list<std::pair<Key,T>>&& reference )
    {
        QHash<Key,T> out;
        for( auto&& pair:reference ) { out.insert( pair.first, pair.second ); }
        return out;
    }

    // for old QT versions there is no QMap constructor from initializer_list
    template<typename Key, typename T>
    QMap<Key,T> makeMap( std::initializer_list<std::pair<Key,T>>&& reference )
    {
        QMap<Key,T> out;
        for( auto&& pair:reference ) { out.insert( pair.first, pair.second ); }
        return out;
    }
    #endif

}

//* fancy qhash for all enum types
template<typename T,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
    uint qHash( const T& value )
{ return qHash(Base::toIntegralType(value)); }

#endif

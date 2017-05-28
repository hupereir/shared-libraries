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

#include <QHash>

namespace Base
{

    template<typename T>
        using underlying_type_t = typename std::underlying_type<T>::type;

    //* convert an strong type enum to integral type
    template<typename T>
    constexpr typename std::underlying_type<T>::type
    toIntegralType(T value) noexcept
    { return static_cast<underlying_type_t<T>>(value);}

}

//* fancy qhash for all enum types
template<typename T,
    typename = typename std::enable_if<std::is_enum<T>::value>::type>
    uint qHash( const T& value )
{ return qHash(Base::toIntegralType(value)); }

#endif

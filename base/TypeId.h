#ifndef TypeId_h
#define TypeId_h

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

namespace Base
{

    // class from integer
    template<int N>
        class MarkerId
    {
        public:
        static constexpr int value = N;
    };

    // class from type
    template<class T>
        class MarkerType
    {
        public:
        using type = T;
    };

    //* map class to integer at compile time
    template<class T, int N>
        class RegisterTypeId: public MarkerId<N>, MarkerType<T>
    {

        private:

        /**
        this ensures that a compiler warning is issued
        when two types are registered with the same integer
        */
        friend MarkerType<T> _type(MarkerId<N>)
        { return MarkerType<T>(); }
    };

    //* default TypeId class
    /**
    to map a class to an integer, just specialize using e.g.
    template<> class TypeId<ClassName>: public RegisterType<ClassName, Int>{};
    */
    template<class T> class TypeId;

}

#endif

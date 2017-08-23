#ifndef Functors_h
#define Functors_h

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

#include <memory>
#include <functional>

namespace Base
{

    namespace Functor
    {

        //* generic unary functor that verifies if a value equals a prediction
        template<class T>
        class SameFTor
        {
            public:

            //* constructor
            explicit SameFTor( const T& prediction):
                prediction_( prediction )
            {}

            //* predicate
            template<class U>
                inline bool operator() (const U& object )
            { return object == prediction_; }

            private:

            T prediction_;

        };

        //* generic unary functor that verifies if a given method returns a specific value
        template <class T, typename R, R (T::*accessor)() const>
            class Unary
        {

            public:

            //* constructor
            template<class U>
                explicit Unary( const U& object ):
                prediction_( (object.*accessor)() )
            {}

            //* constructor
            template<class U>
                explicit Unary( U* pointer ):
                prediction_( (pointer->*accessor)() )
            {}

            //* constructor (specialized)
            explicit Unary( const R& prediction ):
                prediction_( prediction )
            {}

            //* predicate
            template<class U>
                inline bool operator() (const U& object ) const
            { return (object.*accessor)() == prediction_; }

            //* predicate
            template<class U>
                inline bool operator() ( U* pointer ) const
            { return (pointer->*accessor)() == prediction_; }

            //* predicate
            template<class U>
                inline bool operator() (const std::shared_ptr<U>& pointer ) const
            { return (pointer.get()->*accessor)() == prediction_; }

            private:

            //* prediction
            R prediction_;

        };

        template <class T, typename R, R (T::*accessor)() const>
        using UnaryEqual = Unary<T, R, accessor>;

        //* generic unary functor that verifies if a given method is satisfied
        template <class T, bool (T::*accessor)() const, bool prediction = true>
            class UnaryTrue
        {

            public:

            //* predicate
            template<class U>
                inline bool operator() (const U& object ) const
            { return (object.*accessor)() == prediction; }

            //* predicate
            template<class U>
                inline bool operator() ( U* pointer ) const
            { return (pointer->*accessor)() == prediction; }

            //* predicate
            template<class U>
                inline bool operator() (const std::shared_ptr<U>& pointer ) const
            { return (pointer.get()->*accessor)() == prediction; }

        };

        //* generic unary functor that verifies if a given method is not satisfied
        template <class T, bool (T::*accessor)() const>
        using  UnaryFalse = UnaryTrue<T, accessor, false>;

        //* generic binary functor that compares a the result of a given accessor between two objects
        template <class T, typename R, R (T::*accessor)() const, typename Comparator = std::equal_to<R> >
            class Binary
        {

            public:

            //* predicate
            template<class U>
                inline bool operator() (const U& lhs, const U& rhs ) const
            { return c_((lhs.*accessor)(), (rhs.*accessor)()); }

            //* predicate
            template<class U>
                inline bool operator() ( U* lhs, U* rhs ) const
            { return c_((lhs->*accessor)(), (rhs->*accessor)()); }

            private:

            //* comparator
            Comparator c_;

        };

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryEqual = Binary<T, R, accessor>;

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryLess = Binary<T, R, accessor, std::less<R>>;

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryMore = Binary<T, R, accessor, std::greater<R>>;

    }

}

#endif

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

#include "base_export.h"
#include <memory>
#include <functional>

namespace Base
{

    namespace Functor
    {

        //* generic unary functor that verifies if a value equals a prediction
        template<class T, typename Comparator = std::equal_to<T> >
        class BASE_EXPORT SameFTor
        {
            public:

            //* constructor
            explicit SameFTor( const T& prediction):
                prediction_( prediction )
            {}

            //* predicate
            bool operator() (const T& object )
            { return c_(object,prediction_); }

            private:

            //* comparator
            Comparator c_;

            //* prediction
            T prediction_;

        };

        template <class T>
        using DifferFTor = SameFTor<T, std::not_equal_to<T> >;

        template <class T>
        using LessFTor = SameFTor<T, std::less<T> >;

        template <class T>
        using GreaterFTor = SameFTor<T, std::greater<T> >;

        //* generic unary functor that verifies if a given method returns a specific value
        template <class T, typename R, R (T::*accessor)() const, typename Comparator = std::equal_to<R> >
            class BASE_EXPORT Unary
        {

            public:

            //* constructor
            explicit Unary( const R& prediction ):
                prediction_( prediction )
            {}

            //* constructor
            template<
                class BASE_EXPORT U,
                typename = typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value>::type
             >
            explicit Unary( const U& object ):
            prediction_( (object.*accessor)() )
            {}

            //* constructor
            template<
                class BASE_EXPORT U,
                typename = typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value>::type
             >
            explicit Unary( U* pointer ):
            prediction_( (pointer->*accessor)() )
            {}

            //* constructor
            template<
                class BASE_EXPORT U,
                typename = typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value>::type
             >
            explicit Unary( const std::shared_ptr<U>& pointer ):
            prediction_( (pointer.get()->*accessor)() )
            {}

            //* predicate
            bool operator() (const T& object ) const
            { return c_((object.*accessor)(), prediction_); }

            //* predicate
            bool operator() ( T* pointer ) const
            { return c_((pointer->*accessor)(), prediction_); }

            //* predicate
            bool operator() (const std::shared_ptr<T>& pointer ) const
            { return c_((pointer.get()->*accessor)(), prediction_); }

            private:

            //* comparator
            Comparator c_;

            //* prediction
            R prediction_;

        };

        template <class T, typename R, R (T::*accessor)() const>
        using UnaryEqual = Unary<T, R, accessor>;

        template <class T, typename R, R (T::*accessor)() const>
        using UnaryDiffer = Unary<T, R, accessor, std::not_equal_to<R> >;

        template <class T, typename R, R (T::*accessor)() const>
        using UnaryLess = Unary<T, R, accessor, std::less<R> >;

        template <class T, typename R, R (T::*accessor)() const>
        using UnaryMore = Unary<T, R, accessor, std::greater<R> >;

        //* generic unary functor that verifies if a given method is satisfied
        template <class T, bool (T::*accessor)() const, bool prediction = true>
            class BASE_EXPORT UnaryTrue
        {

            public:

            //* predicate
            bool operator() (const T& object ) const
            { return (object.*accessor)() == prediction; }

            //* predicate
            bool operator() ( T* pointer ) const
            { return (pointer->*accessor)() == prediction; }

            //* predicate
            bool operator() (const std::shared_ptr<T>& pointer ) const
            { return (pointer.get()->*accessor)() == prediction; }

        };

        //* generic unary functor that verifies if a given method is not satisfied
        template <class T, bool (T::*accessor)() const>
        using  UnaryFalse = UnaryTrue<T, accessor, false>;

        //* generic binary functor that compares a the result of a given accessor between two objects
        template <class T, typename R, R (T::*accessor)() const, typename Comparator = std::equal_to<R> >
            class BASE_EXPORT Binary
        {

            public:

            //* predicate
            bool operator() (const T& lhs, const T& rhs ) const
            { return c_((lhs.*accessor)(), (rhs.*accessor)()); }

            //* predicate
            bool operator() ( T* lhs, T* rhs ) const
            { return c_((lhs->*accessor)(), (rhs->*accessor)()); }


            //* predicate
            bool operator() ( const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs ) const
            { return c_((lhs.get()->*accessor)(), (rhs.get()->*accessor)()); }

            private:

            //* comparator
            Comparator c_;

        };

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryEqual = Binary<T, R, accessor>;

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryLess = Binary<T, R, accessor, std::less<R> >;

        template <class T, typename R, R (T::*accessor)() const>
        using BinaryMore = Binary<T, R, accessor, std::greater<R> >;

    }

}

#endif

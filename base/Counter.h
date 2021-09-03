#ifndef Counter_h
#define Counter_h

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
#include "CounterMap.h"

#include <QString>

namespace Base
{
    template<typename T>
    class Counter
    {

        public:

        //* construtor
        explicit Counter( const QString& name )
        {
            if( !initialized )
            {
                initialized = true;
                Base::CounterMap::get().insert( name, &count );
            }

            count++;
        }

        //* copy constructor
        explicit Counter(const Counter&)
        { count++; }

        //* assignment operator
        Counter& operator=(const Counter&) = default;
        
        //* destructor
        ~Counter()
        { count--; }

        private:

        //* count
        static bool initialized;
        static int count;
    };

}

template <typename T> bool Base::Counter<T>::initialized( false );
template <typename T> int Base::Counter<T>::count( 0 );

#endif

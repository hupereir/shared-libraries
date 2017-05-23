#ifndef SshSingleton_h
#define SshSingleton_h

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

#include "Counter.h"

namespace Ssh
{

    //* singleton, to handle ssh allocation and freeing
    class Singleton: private Base::Counter<Singleton>
    {
        public:

        //* destructor
        ~Singleton( void );

        //* accessor
        static Singleton& get( void );

        //* true if initialized
        bool initialized( void ) const
        { return initialized_; }

        //* initialize
        void initialize( void );

        private:

        //* constructor
        Singleton( void );

        //* true if initalized
        bool initialized_ = false;

    };

}

#endif

#ifndef CompositeEngine_h
#define CompositeEngine_h

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

#include "NonCopyable.h"
#include "base_transparency_export.h"

namespace Transparency
{

    //* initialize compositing, if available
    class BASE_TRANSPARENCY_EXPORT CompositeEngine: private Base::NonCopyable<CompositeEngine>
    {
        public:

        //* singleton
        static CompositeEngine& get();

        //* availability
        bool isAvailable()
        {
            _initialize();
            return available_;
        }

        protected:

        //* initialize
        void _initialize();

        //* returns true if composition is enabled
        bool _compositingAvailable() const;

        private:

        //* constructor
        explicit CompositeEngine();

        //* validity
        bool available_;

        //* initialized
        bool initialized_;

    };

};

#endif

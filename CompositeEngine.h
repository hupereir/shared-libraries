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

namespace Transparency
{

    //! initialize compositing, if available
    class CompositeEngine
    {
        public:

        //! singleton
        static CompositeEngine& get( void );

        //! availability
        bool isAvailable( void )
        {
            _initialize();
            return available_;
        }

        protected:

        //! initialize
        void _initialize( void );

        //! returns true if composition is enabled
        bool _compositingAvailable( void ) const;

        private:

        //! constructor
        CompositeEngine( void );

        //! validity
        bool available_;

        //! initialized
        bool initialized_;

    };

};

#endif

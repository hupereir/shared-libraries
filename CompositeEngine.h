#ifndef CompositeEngine_h
#define CompositeEngine_h

// $Id$

/***************************************************************************
*   Copyright (C) 2006 by Helio Chissini de Castro   *
*   helio@mandriva.com   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

namespace TRANSPARENCY
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

        //! enability
        bool isEnabled( void )
        { return isAvailable() && enabled_; }

        //! enability
        bool setEnabled( bool value );

        protected:

        //! initialize
        void _initialize( void );

        //! returns true if composition is enabled
        bool _compositingEnabled( void ) const;

        private:

        //! constructor
        CompositeEngine( void );

        //! validity
        bool available_;

        //! enability
        bool enabled_;

        //! initialized
        bool initialized_;

    };

};

#endif

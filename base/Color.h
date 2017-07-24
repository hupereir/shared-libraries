#ifndef Color_h
#define Color_h

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
#include "QOrderedSet.h"

#include <QColor>
#include <QTextStream>

namespace Base
{

    // color utility
    class Color: private Base::Counter<Color>
    {

        public:

        //* universal constructor
        template< typename... Args >
            explicit Color( Args&&... args ):
            Counter( "Base::Color" ),
            value_( std::forward<Args>(args)... )
        {}

        //* destructor
        virtual ~Color() = default;

        //* sorted set of colors
        using Set = QOrderedSet<Color>;

        //*@name accessors
        //@{

        //* conversion to QColor
        operator const QColor& () const { return value_; }

        //* mutable accessor
        const QColor& get() const { return value_; }

        //* validity
        bool isValid() const { return value_.isValid(); }

        //* merge color with intensity to this one
        Color merged( const QColor& color, qreal intensity = 0.5 )
        {
            Color copy( *this );
            return copy.merge( color, intensity );
        }

        //*@}

        //*@name modifiers
        //@{

        //* mutable accessor
        QColor& get() { return value_; }

        //* merge color with intensity to this one
        Color& merge( const QColor& , qreal = 0.5 );

        //* add alpha
        Color& addAlpha( qreal = 0.5 );

        //@}

        private:

        QColor value_;

    };

    //* equal to operator
    inline bool operator == (const Color& first, const Color& second)
    { return first.get() == second.get(); }

    //* less than operator
    bool operator < (const Color&, const Color&);

}

//* streamers
QTextStream& operator << ( QTextStream&, const Base::Color& );
QTextStream& operator >> ( QTextStream&, Base::Color& );

#endif

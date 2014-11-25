#ifndef Color_h
#define Color_h

// $Id$

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

#include "QOrderedSet.h"

#include <QColor>
#include <QTextStream>

namespace Base
{

    // color utility
    class Color: public QColor
    {

        public:

        //! constructor
        Color( void )
        {}

        //! constructor
        Color( const QString& value )
        { setNamedColor( value ); }

        //! constructor
        Color( const QColor& color ):
            QColor( color )
        {}

        //! merge argument color with intensity to this one
        QColor merge( const QColor& , qreal = 0.5 ) const;

        //! add alpha
        QColor addAlpha( qreal = 0.5 ) const;

        //! less than operator
        bool operator < (const Color& ) const;

        //* sorted set of colors
        using Set = QOrderedSet<Color>;

    };
    
}

//! streamers
QTextStream& operator << ( QTextStream&, const Base::Color& );
QTextStream& operator >> ( QTextStream&, Base::Color& );

#endif

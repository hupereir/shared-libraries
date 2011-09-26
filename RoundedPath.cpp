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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
\file    Roundedcpp
\brief   rounded QPainterPath based on polygon
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include <cassert>
#include <cmath>

#include "RoundedPath.h"
#include "Debug.h"

using namespace std;

//______________________________________________________
RoundedPath::RoundedPath( QPolygonF polygon, const int& border )
{

    enum Direction { HORIZONTAL, VERTICAL };
    Direction direction( HORIZONTAL );

    // check polygon size
    assert( polygon.size() >= 5 );
    assert( border > 0 );

    QPointF first( polygon[0] + QPointF( border, 0 ) );
    QPointF current( first );
    moveTo( first );

    for( int i = 1; i < polygon.size(); i++ )
    {

        QPointF corner( polygon[i] );
        QPointF next( i < polygon.size()-1 ? polygon[i+1]:first );
        QPointF offset;

        // move close to next corner
        if( std::abs( corner.y() - current.y() ) < 1 )
        {
            direction = HORIZONTAL;
            offset = QPointF( corner.x() < current.x() ? border:-border, 0 );

        } else {

            direction = VERTICAL;
            offset = QPointF( 0, corner.y() < current.y() ? border:-border );

        }

        current = corner+offset;
        lineTo( current );

        // check that direction is changed
        if( ( std::abs( next.y() - corner.y() ) < 1 && direction == HORIZONTAL ) || ( std::abs( next.x() - corner.x() ) < 1 && direction == VERTICAL ) ) continue;

        // curve around next corner
        if( std::abs( next.y() - corner.y() ) < 1 ) offset = QPointF( corner.x() < next.x() ? border:-border, 0 );
        else offset = QPointF( 0, corner.y() < next.y() ? border:-border );
        current = corner+offset;
        quadTo( corner, current );

    }

}

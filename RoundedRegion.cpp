
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

/*!
  \file    RoundedRegion.cpp
  \brief   rounded QRegion based on a rect
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/


#include "RoundedRegion.h"

//_________________________________________________________
RoundedRegion::RoundedRegion( QRect rect, Corners corners ):
  QRegion( rect )
{

  // top left
  if( corners & TopLeft )
  {
    const QPoint& point( rect.topLeft() );
    *this -= QRegion(point.x(), point.y()+0, 5, 1);
    *this -= QRegion(point.x(), point.y()+1, 3, 1);
    *this -= QRegion(point.x(), point.y()+2, 2, 1);
    *this -= QRegion(point.x(), point.y()+3, 1, 2);
  }

  // top right
  if( corners & TopRight )
  {
    const QPoint& point( rect.topRight() );
    *this -= QRegion(point.x()-4, point.y()+0, 5, 1);
    *this -= QRegion(point.x()-2, point.y()+1, 3, 1);
    *this -= QRegion(point.x()-1, point.y()+2, 2, 1);
    *this -= QRegion(point.x()-0, point.y()+3, 1, 2);
  }

  // bottom left
  if( corners & BottomLeft )
  {
    const QPoint& point( rect.bottomLeft() );
    *this -= QRegion(point.x(), point.y()-0, 5, 1);
    *this -= QRegion(point.x(), point.y()-1, 3, 1);
    *this -= QRegion(point.x(), point.y()-2, 2, 1);
    *this -= QRegion(point.x(), point.y()-4, 1, 2);
  }


  // bottom right
  if( corners & BottomRight )
  {
    const QPoint& point( rect.bottomRight() );
    *this -= QRegion(point.x()-4, point.y()-0, 5, 1);
    *this -= QRegion(point.x()-2, point.y()-1, 3, 1);
    *this -= QRegion(point.x()-1, point.y()-2, 2, 1);
    *this -= QRegion(point.x()-0, point.y()-4, 1, 2);
  }

}

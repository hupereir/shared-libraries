#ifndef RoundedRegion_h
#define RoundedRegion_h

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
  \file    RoundedRegion.h
  \brief   rounded QRegion based on a rect
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QRegion>
#include <QRect>

//! rounded QRegion based on a rect
class RoundedRegion: public QRegion
{

  public:

  //! corners
  enum Corner
  {
    NONE = 0,
    TOP_LEFT = 1<<0,
    TOP_RIGHT = 1<<1,
    BOTTOM_LEFT  = 1<<2,
    BOTTOM_RIGHT = 1<<3,
    TOP = TOP_LEFT | TOP_RIGHT,
    BOTTOM = BOTTOM_LEFT | BOTTOM_RIGHT,
    LEFT = TOP_LEFT | BOTTOM_LEFT,
    RIGHT = TOP_RIGHT | BOTTOM_RIGHT,
    ALL = TOP | BOTTOM
  };

  //! constructor
  RoundedRegion( QRect, unsigned int corners );

};

#endif

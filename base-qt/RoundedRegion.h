#ifndef RoundedRegion_h
#define RoundedRegion_h

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

#include <QRegion>
#include <QRect>

//! rounded QRegion based on a rect
class RoundedRegion: public QRegion
{

    public:

    //! corners
    enum Corner
    {
        None = 0,
        TopLeft = 1<<0,
        TopRight = 1<<1,
        BottomLeft  = 1<<2,
        BottomRight = 1<<3,
        Top = TopLeft | TopRight,
        Bottom = BottomLeft | BottomRight,
        Left = TopLeft | BottomLeft,
        Right = TopRight | BottomRight,
        All = Top | Bottom
    };

    Q_DECLARE_FLAGS( Corners, Corner );

    //! constructor
    RoundedRegion( QRect, Corners = Corner::All );

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RoundedRegion::Corners )

#endif

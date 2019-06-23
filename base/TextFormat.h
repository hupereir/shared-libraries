#ifndef TextFormat_h
#define TextFormat_h

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

#include <QFlags>

namespace TextFormat
{

    //* format bit enumaration
    enum Flag
    {
        Default = 0,
        Bold = 1<<0,
        Italic = 1<<1,
        Underline = 1<<2,
        Strike = 1<<3,
        Overline = 1<<4,
        LargeFont = 1<<5

    };

    Q_DECLARE_FLAGS( Flags, Flag )

}

Q_DECLARE_OPERATORS_FOR_FLAGS( TextFormat::Flags )


#endif

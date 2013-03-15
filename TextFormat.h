// $Id$
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

#include <QFlags>

namespace FORMAT
{

    //! format bit enumaration
    enum TextFormat
    {
        Default = 0,
        Bold = 1<<0,
        Italic = 1<<1,
        Underline = 1<<2,
        Strike = 1<<3,
        Overline = 1<<4

    };

    Q_DECLARE_FLAGS( TextFormatFlags, TextFormat )

}

Q_DECLARE_OPERATORS_FOR_FLAGS( FORMAT::TextFormatFlags )


#endif

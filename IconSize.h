#ifndef IconSize_h
#define IconSize_h

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

#include <QtGui/QWidget>
#include <QtCore/QSize>
#include <QtCore/QMap>

//! default icon sizes
class IconSize: public QSize
{

    public:


    //! icon sizes
    enum Size
    {
        Default = 0,
        Small = 16,
        Medium = 22,
        Large = 32,
        Huge = 48,
        Maximum = 256
    };

    //! constructor
    IconSize( QWidget* parent, Size size );

    //! constructor
    IconSize( Size size );

    //! map text to icon size
    typedef QMap<Size, QString> Map;

    //! text to icon size
    static Map& map( void );

};

#endif

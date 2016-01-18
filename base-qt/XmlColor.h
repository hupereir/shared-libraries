#ifndef XmlColor_h
#define XmlColor_h

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

#include "Color.h"

#include <QDomDocument>
#include <QDomElement>
#include <QString>

//! some specific Xml definitions
namespace Base
{
    namespace Xml
    {
        static const QString ColorList( "Colors" );
        static const QString Color = "Color";
    }
}

//! xml implementation of the color object
class XmlColor: public Base::Color
{

    public:

    //! copy constructor
    XmlColor( const Base::Color& other ):
        Base::Color( other )
    {}

    //! constructor from DOM
    XmlColor( const QDomElement& );

    //! create dom element
    QDomElement domElement( QDomDocument& parent ) const;

};

#endif

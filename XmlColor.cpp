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


#include "XmlColor.h"

//! some specific Xml definitions
namespace Base
{
    namespace Xml
    {
        static const QString Name = "Name";
        static const QString Red = "r";
        static const QString Green = "g";
        static const QString Blue = "b";
        static const QString Alpha = "a";
    }

}

//________________________________________________
XmlColor::XmlColor( const QDomElement& element )
{

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( int i=0; i<attributes.count(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == Base::Xml::Name ) setNamedColor( attribute.value() );
        else if( attribute.name() == Base::Xml::Red ) setRed( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::Green ) setGreen( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::Blue ) setBlue( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::Alpha ) setAlpha( attribute.value().toInt() );
    }

}

//________________________________________________
QDomElement XmlColor::domElement( QDomDocument& document ) const
{

    QDomElement element = document.createElement( Base::Xml::Color );
    element.setAttribute( Base::Xml::Red, QString::number( red() ) );
    element.setAttribute( Base::Xml::Green, QString::number( green() ) );
    element.setAttribute( Base::Xml::Blue, QString::number( blue() ) );
    element.setAttribute( Base::Xml::Alpha, QString::number( alpha() ) );

    return element;

}

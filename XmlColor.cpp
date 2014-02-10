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


#include "XmlColor.h"

//! some specific Xml definitions
namespace Base
{
    namespace Xml
    {
        static const QString NAME = "Name";
        static const QString RED = "r";
        static const QString GREEN = "g";
        static const QString BLUE = "b";
        static const QString ALPHA = "a";
    }

}

//________________________________________________
XmlColor::XmlColor( const QDomElement& element )
{

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == Base::Xml::NAME ) setNamedColor( attribute.value() );
        else if( attribute.name() == Base::Xml::RED ) setRed( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::GREEN ) setGreen( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::BLUE ) setBlue( attribute.value().toInt() );
        else if( attribute.name() == Base::Xml::ALPHA ) setAlpha( attribute.value().toInt() );
    }

}

//________________________________________________
QDomElement XmlColor::domElement( QDomDocument& document ) const
{

    QDomElement element = document.createElement( Base::Xml::COLOR );
    element.setAttribute( Base::Xml::RED, QString::number( red() ) );
    element.setAttribute( Base::Xml::GREEN, QString::number( green() ) );
    element.setAttribute( Base::Xml::BLUE, QString::number( blue() ) );
    element.setAttribute( Base::Xml::ALPHA, QString::number( alpha() ) );

    return element;

}

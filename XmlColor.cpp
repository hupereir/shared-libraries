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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*******************************************************************************/


#include "XmlColor.h"

//! some specific Xml definitions
namespace BASE
{
    namespace XML
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
        if( attribute.name() == BASE::XML::NAME ) setNamedColor( attribute.value() );
        else if( attribute.name() == BASE::XML::RED ) setRed( attribute.value().toInt() );
        else if( attribute.name() == BASE::XML::GREEN ) setGreen( attribute.value().toInt() );
        else if( attribute.name() == BASE::XML::BLUE ) setBlue( attribute.value().toInt() );
        else if( attribute.name() == BASE::XML::ALPHA ) setAlpha( attribute.value().toInt() );
    }

}

//________________________________________________
QDomElement XmlColor::domElement( QDomDocument& document ) const
{

    QDomElement element = document.createElement( BASE::XML::COLOR );
    element.setAttribute( BASE::XML::RED, QString().setNum( red() ) );
    element.setAttribute( BASE::XML::GREEN, QString().setNum( green() ) );
    element.setAttribute( BASE::XML::BLUE, QString().setNum( blue() ) );
    element.setAttribute( BASE::XML::ALPHA, QString().setNum( alpha() ) );

    return element;

}

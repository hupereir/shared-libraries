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

#include "XmlOption.h"

namespace Base
{
    //* some XML definitions specific to Option management
    namespace Xml
    {

        static const QString Name = "Name";
        static const QString Flags = "flags";
        static const QString Front = "Front";

    }

}

//________________________________________________
XmlOption::XmlOption( const QDomElement& element )
{
    Debug::Throw( "XmlOption::XmlOption.\n" );

    // old implementation (kept for backward compatibility
    // element name is option name
    if( element.nodeName() != Base::Xml::Option ) setName( element.nodeName() );

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( int i=0; i<attributes.count(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == Base::Xml::Name ) setName( attribute.value() );
        else if( attribute.name() == Base::Xml::Value ) setRaw( attribute.value() );
        else if( attribute.name() == Base::Xml::Flags ) setFlags( (Option::Flags) attribute.value().toInt() );
        else Debug::Throw() << "XmlOption::XmlOption - unrecognized attribute " << attribute.name() << ".\n";

    }

    // parse children elements
    for(QDomNode childNode = element.firstChild(); !childNode.isNull(); childNode = childNode.nextSibling() )
    {
        QDomElement childElement = childNode.toElement();
        if( childElement.tagName() == Base::Xml::Name ) setName( childElement.text() );
        else if( childElement.tagName() == Base::Xml::Value ) setRaw( childElement.text() );
        else if( childElement.tagName() == Base::Xml::Flags ) setFlags( (Option::Flags) childElement.text().toInt() );
        else Debug::Throw() << "XmlOption::XmlOption - unrecognized child " << childElement.tagName() << ".\n";

    }

}

//________________________________________________
QDomElement XmlOption::domElement( QDomDocument& document ) const
{

    Debug::Throw() << "XmlOption::DomElement - " << name() << " - " << raw() << endl;

    QDomElement out = document.createElement( Base::Xml::Option );
    out.setAttribute( Base::Xml::Name, name() );
    out.setAttribute( Base::Xml::Flags, QString::number( flags() ) );
    out.setAttribute( Base::Xml::Value, QString::fromUtf8( raw() ) );

    return out;

}

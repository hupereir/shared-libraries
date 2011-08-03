
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
*
*******************************************************************************/

/*!
\file XmlOption.cpp
\brief Xml implementation of the Option object
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "XmlOption.h"
#include "XmlString.h"
using namespace std;

//________________________________________________
XmlOption::XmlOption( const QDomElement& element )
{
    Debug::Throw( "XmlOption::XmlOption.\n" );

    // old implementation (kept for backward compatibility
    // element name is option name
    if( element.nodeName() != OPTIONS::OPTION ) setName( element.nodeName() );

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == OPTIONS::NAME ) setName( attribute.value() );
        else if( attribute.name() == OPTIONS::VALUE ) setRaw( XmlString( attribute.value() ).toText() );
        else if( attribute.name() == OPTIONS::COMMENTS ) setComments( XmlString( attribute.value() ).toText() );
        else if( attribute.name() == OPTIONS::FLAGS ) {

            setFlags( (unsigned int) attribute.value().toInt() );

        } else Debug::Throw(0) << "XmlOption::XmlOption - unrecognized attribute " << attribute.name() << ".\n";

    }

    // parse children elements
    for(QDomNode childNode = element.firstChild(); !childNode.isNull(); childNode = childNode.nextSibling() )
    {
        QDomElement childElement = childNode.toElement();
        if( childElement.tagName() == OPTIONS::NAME ) setName( childElement.text() );
        else if( childElement.tagName() == OPTIONS::VALUE ) setRaw( XmlString( childElement.text() ).toText() );
        else if( childElement.tagName() == OPTIONS::COMMENTS ) setComments( XmlString( childElement.text() ).toText() );
        else if( childElement.tagName() == OPTIONS::FLAGS ) setFlags( (unsigned int) childElement.text().toInt() );
        else Debug::Throw(0) << "XmlOption::XmlOption - unrecognized child " << childElement.tagName() << ".\n";

    }

}

//________________________________________________
QDomElement XmlOption::domElement( QDomDocument& document ) const
{

    Debug::Throw() << "XmlOption::DomElement - " << name() << " - " << raw() << endl;

    QDomElement out = document.createElement( OPTIONS::OPTION );

    out.
        appendChild( document.createElement( OPTIONS::NAME ) ).
        appendChild( document.createTextNode( name() ) );

    out.
        appendChild( document.createElement( OPTIONS::VALUE ) ).
        appendChild( document.createTextNode( XmlString( raw() ).toXml() ) );

    out.
        appendChild( document.createElement( OPTIONS::FLAGS ) ).
        appendChild( document.createTextNode( QString().setNum( flags() ) ) );

    if( comments().size() )
    {
        out.
            appendChild( document.createElement( OPTIONS::COMMENTS ) ).
            appendChild( document.createTextNode( XmlString( comments() ).toXml() ) );
    }

    return out;

}

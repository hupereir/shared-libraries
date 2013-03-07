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

#include "Debug.h"
#include "XmlTimeStamp.h"

namespace XML
{

    static const QString TIME( "time" );

}

//______________________________________________________
XmlTimeStamp::XmlTimeStamp( const QDomElement& element )
{
    Debug::Throw( "XmlTimeStamp::XmlTimeStamp.\n" );

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == XML::TIME ) setTime( attribute.value().toUInt() );
    }

};

//_______________________________________________________
QDomElement XmlTimeStamp::domElement( const QString& name, QDomDocument& document ) const
{
    Debug::Throw( "XmlTimeStamp::domElement.\n" );
    QDomElement out( document.createElement( name ) );
    out.setAttribute( XML::TIME, QString().setNum( unixTime() ) );
    return out;
}

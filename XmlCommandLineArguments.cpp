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

#include "XmlCommandLineArguments.h"
#include "XmlString.h"

namespace XML
{
    const QString ARGUMENT( "c" );
}

//______________________________________________________
XmlCommandLineArguments::XmlCommandLineArguments( const QDomElement& element )
{

    Debug::Throw( "XmlCommandLineArguments::XmlCommandLineArguments" );

    // parse children elements
    for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
    {
        QDomElement child_element = child_node.toElement();
        if( child_element.isNull() ) continue;

        if( child_element.tagName() == XML::ARGUMENT ) { append( XmlString( child_element.text() ).toText() ); }
        else Debug::Throw() << "XmlCommandLineArguments::XmlCommandLineArguments - unrecognized child: " << child_element.tagName() << endl;

    }

};

//_______________________________________________________
QDomElement XmlCommandLineArguments::domElement( const QString& name, QDomDocument& parent ) const
{
    QDomElement out( parent.createElement( name ) );
    foreach( const QString& argument, *this )
    {
        out.
            appendChild( parent.createElement( XML::ARGUMENT ) ).
            appendChild( parent.createTextNode( XmlString( argument ).toXml() ) );
    }
    return out;
}

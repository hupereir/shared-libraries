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

#include "XmlCommandLineArguments.h"

namespace Xml
{
    const QString Argument( "c" );
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

        if( child_element.tagName() == Xml::Argument ) { append( child_element.text() ); }
        else Debug::Throw() << "XmlCommandLineArguments::XmlCommandLineArguments - unrecognized child: " << child_element.tagName() << endl;

    }

};

//_______________________________________________________
QDomElement XmlCommandLineArguments::domElement( const QString& name, QDomDocument& parent ) const
{
    QDomElement out( parent.createElement( name ) );
    for( const auto& argument:get() )
    {
        out.
            appendChild( parent.createElement( Xml::Argument ) ).
            appendChild( parent.createTextNode( argument ) );
    }
    return out;
}

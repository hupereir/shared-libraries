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
* Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*******************************************************************************/

#include "XmlDocument.h"

#include "Debug.h"
#include "XmlError.h"

//___________________________________________________________________
bool XmlDocument::setContent( QFile* file, XmlError& error )
{
    Q_ASSERT( file );
    error.clear();
    error.setFile( file->fileName() );
    return QDomDocument::setContent( file, &error.error(), &error.line(), &error.column() );
}

//___________________________________________________________________
bool XmlDocument::setContent( QIODevice* device, XmlError& error )
{
    Q_ASSERT( device );
    error.clear();
    return QDomDocument::setContent( device, &error.error(), &error.line(), &error.column() );
}

//___________________________________________________________________
bool XmlDocument::setContent( const QString& content, XmlError& error )
{
    Q_ASSERT( file );
    error.clear();
    return QDomDocument::setContent( content, &error.error(), &error.line(), &error.column() );
}

//___________________________________________________________________
void XmlDocument::replaceChild( QDomElement& element )
{

    static const QString topNodeTagName = "Resources";
    if( documentElement().tagName() != topNodeTagName )
    {

        Debug::Throw( "XmlDocument - creating relevant topNode.\n" );

        clear();
        appendChild( createElement( topNodeTagName ) );
        documentElement().appendChild( element );
        return;

    }

    // find previous options element
    QDomNodeList children( elementsByTagName( element.tagName() ) );
    if( !children.isEmpty() )
    {

        Debug::Throw() << "XmlDocument - replacing node " << element.tagName() << endl;
        documentElement().replaceChild( element, children.at(0) );

    } else {

        Debug::Throw() << "XmlDocument - creating node " << element.tagName() << endl;
        documentElement().appendChild( element );

    }

    return;

}

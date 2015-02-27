
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

#include "XmlDocument.h"

#include "Debug.h"
#include "XmlError.h"

//___________________________________________________________________
XmlDocument::XmlDocument( void ):
    Counter( "XmlDocument" ),
    topNodeTagName_( "Resources" )
{}

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
bool XmlDocument::setContent( const QByteArray& content, XmlError& error )
{
    error.clear();
    return QDomDocument::setContent( content, &error.error(), &error.line(), &error.column() );
}

//___________________________________________________________________
bool XmlDocument::setContent( const QString& content, XmlError& error )
{
    error.clear();
    return QDomDocument::setContent( content, &error.error(), &error.line(), &error.column() );
}

//___________________________________________________________________
void XmlDocument::replaceChild( QDomElement& element )
{

    if( documentElement().tagName() != topNodeTagName_ )
    {

        clear();
        Debug::Throw() << "XmlDocument - creating document element named " << topNodeTagName_ << endl;
        appendChild( createElement( topNodeTagName_ ) );
        documentElement().appendChild( element );
        return;

    }

    // find previous options element
    QDomNodeList children( elementsByTagName( element.tagName() ) );
    if( !children.isEmpty() )
    {

        Debug::Throw() << "XmlDocument -"
            << " replacing node " << element.tagName()
            << " inside " << documentElement().tagName()
            << endl;
        documentElement().replaceChild( element, children.at(0) );

    } else {

        Debug::Throw() << "XmlDocument -"
            << " creating node " << element.tagName()
            << " inside " << documentElement().tagName()
            << endl;
        documentElement().appendChild( element );

    }

    return;

}

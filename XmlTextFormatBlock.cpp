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

#include "XmlTextFormatBlock.h"
#include "XmlColor.h"
#include "Debug.h"

namespace Format
{

    namespace Xml
    {

        static const QString Format = "format";
        static const QString Color = "color";
        static const QString HRef = "href";
        static const QString Begin = "begin";
        static const QString End = "end";

    }

    //____________________________________
    XmlTextFormatBlock::XmlTextFormatBlock( const QDomElement& element )
    {

        // parse attributes
        QDomNamedNodeMap attributes( element.attributes() );
        for( unsigned int i=0; i<attributes.length(); i++ )
        {
            QDomAttr attribute( attributes.item( i ).toAttr() );
            if( attribute.isNull() ) continue;
            QString name( attribute.name() );
            QString value( attribute.value() );

            // nominal tags
            if( name == Xml::Begin ) setBegin( value.toInt() );
            else if( name == Xml::End ) setEnd( value.toInt() );

            // format
            else if( name == Xml::Format ) setFormat( (TextFormatFlags) value.toInt() );
            else if( name == Xml::Color ) setColor( value );
            else if( name == Xml::HRef ) setHRef( value );

            else Debug::Throw(0) << "XmlTextFormatBlock::XmlTextFormatBlock - unrecognized text format attribute: \"" << name << "\"\n";
        }

    }

    //__________________________________________________________
    QDomElement XmlTextFormatBlock::domElement( QDomDocument& parent ) const
    {
        QDomElement out( parent.createElement( Xml::Tag ) );
        out.setAttribute( Xml::Begin, QString::number(begin()) );
        out.setAttribute( Xml::End, QString::number(end()) );
        out.setAttribute( Xml::Format, QString::number(format()) );
        if( color().isValid() ) out.setAttribute( Xml::Color, color().name() );
        if( !href().isEmpty() ) out.setAttribute( Xml::HRef, href() );
        return out;
    }

}

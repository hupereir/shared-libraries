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

        static const QString FORMAT = "format";
        static const QString COLOR = "color";
        static const QString HREF = "href";
        static const QString BEGIN = "begin";
        static const QString END = "end";

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
            if( name == Xml::BEGIN ) setBegin( value.toInt() );
            else if( name == Xml::END ) setEnd( value.toInt() );

            // format
            else if( name == Xml::FORMAT ) setFormat( (TextFormatFlags) value.toInt() );
            else if( name == Xml::COLOR ) setColor( value );
            else if( name == Xml::HREF ) setHRef( value );

            else Debug::Throw(0) << "XmlTextFormatBlock::XmlTextFormatBlock - unrecognized text format attribute: \"" << name << "\"\n";
        }

    }

    //__________________________________________________________
    QDomElement XmlTextFormatBlock::domElement( QDomDocument& parent ) const
    {
        QDomElement out( parent.createElement( Xml::TAG ) );
        out.setAttribute( Xml::BEGIN, QString::number(begin()) );
        out.setAttribute( Xml::END, QString::number(end()) );
        out.setAttribute( Xml::FORMAT, QString::number(format()) );
        if( color().isValid() ) out.setAttribute( Xml::COLOR, color().name() );
        if( !href().isEmpty() ) out.setAttribute( Xml::HREF, href() );
        return out;
    }

}

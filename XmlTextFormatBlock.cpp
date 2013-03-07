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

#include "XmlTextFormatBlock.h"
#include "XmlColor.h"
#include "Debug.h"

namespace FORMAT
{

    namespace XML
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
            if( name == XML::BEGIN ) setBegin( value.toInt() );
            else if( name == XML::END ) setEnd( value.toInt() );

            // format
            else if( name == XML::FORMAT ) setFormat( (TextFormatFlags) value.toInt() );
            else if( name == XML::COLOR ) setColor( value );
            else if( name == XML::HREF ) setHRef( value );

            else Debug::Throw(0) << "XmlTextFormatBlock::XmlTextFormatBlock - unrecognized text format attribute: \"" << name << "\"\n";
        }

    }

    //__________________________________________________________
    QDomElement XmlTextFormatBlock::domElement( QDomDocument& parent ) const
    {
        QDomElement out( parent.createElement( XML::TAG ) );
        out.setAttribute( XML::BEGIN, QString().setNum(begin()) );
        out.setAttribute( XML::END, QString().setNum(end()) );
        out.setAttribute( XML::FORMAT, QString().setNum(format()) );
        if( color().isValid() ) out.setAttribute( XML::COLOR, color().name() );
        if( !href().isEmpty() ) out.setAttribute( XML::HREF, href() );
        return out;
    }

}

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

#include "XmlFileRecord.h"
#include "Str.h"
#include "XmlString.h"

namespace FILERECORD
{

    //! some XML definitions specific to Option management
    namespace XML
    {
        static const QString FILE = "file";
        static const QString TIME = "time";
        static const QString FLAGS = "flags";
        static const QString VALID= "valid";
        static const QString PROPERTY= "property";
        static const QString NAME= "name";
        static const QString VALUE= "value";
    }
}

//_______________________________________________
XmlFileRecord::XmlFileRecord( const QDomElement& element )
{
    Debug::Throw( "XmlFileRecord::XmlFileRecord.\n" );

    // load attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() || attribute.name().isEmpty() ) continue;
        if( attribute.name() == FILERECORD::XML::FILE ) setFile( XmlString( attribute.value() ).toText() );
        else if( attribute.name() == FILERECORD::XML::TIME ) setTime( attribute.value().toInt() );
        else if( attribute.name() == FILERECORD::XML::FLAGS ) setFlags( attribute.value().toUInt() );
        else if( attribute.name() == FILERECORD::XML::VALID ) setValid( attribute.value().toInt() );
        else addProperty( attribute.name(), attribute.value() );
    }

    // parse children elements
    for(QDomNode childNode = element.firstChild(); !childNode.isNull(); childNode = childNode.nextSibling() )
    {
        QDomElement childElement = childNode.toElement();
        if( childElement.isNull() ) continue;

        QString tagName( childElement.tagName() );
        if( tagName == FILERECORD::XML::PROPERTY )
        {

            std::pair< QString, QString > property;

            // load attributes
            QDomNamedNodeMap attributes( childElement.attributes() );
            for( unsigned int i=0; i<attributes.length(); i++ )
            {

                QDomAttr attribute( attributes.item( i ).toAttr() );
                if( attribute.isNull() || attribute.name().isEmpty() ) continue;

                if( attribute.name() == FILERECORD::XML::NAME ) property.first = XmlString( attribute.value() ).toText();
                else if( attribute.name() == FILERECORD::XML::VALUE ) property.second = XmlString( attribute.value() ).toText();
                else Debug::Throw(0) << "XmlFileRecord::XmlFileRecord - unrecognized attribute " << attribute.name() << endl;

            }

            if( !( property.first.isEmpty() || property.second.isEmpty() ) )
            { addProperty( property.first, property.second ); }

        } else Debug::Throw(0) << "XmlFileRecord::XmlFileRecord - unrecognized child " << childElement.tagName() << endl;
    }

}

//_______________________________________________
QDomElement XmlFileRecord::domElement( QDomDocument& parent ) const
{
    Debug::Throw( "XmlFileRecord::domElement.\n" );
    QDomElement out( parent.createElement( FILERECORD::XML::RECORD ) );
    out.setAttribute( FILERECORD::XML::FILE, XmlString( file() ).toXml() );
    out.setAttribute( FILERECORD::XML::TIME, Str().assign<int>( XmlFileRecord::time() ) );
    out.setAttribute( FILERECORD::XML::VALID, Str().assign<bool>( isValid() ) );

    if( flags() ) out.setAttribute( FILERECORD::XML::FLAGS, Str().assign<unsigned int>( flags() ) );

    for( PropertyMap::const_iterator iter = properties().begin(); iter != properties().end(); ++iter )
    {
        QDomElement property( parent.createElement( FILERECORD::XML::PROPERTY ) );
        property.setAttribute( FILERECORD::XML::NAME, XmlString( PropertyId::get(iter.key()) ).toXml() );
        property.setAttribute( FILERECORD::XML::VALUE, XmlString( iter.value() ) );
        out.appendChild( property );
    }

    return out;
}

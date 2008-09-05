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
   \file XmlFileRecord.cpp
   \brief xml interface to FileRecord
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "XmlFileRecord.h"
#include "Str.h" 
#include "XmlString.h" 

using namespace std;

//!@name XML names
//@{
const QString XmlFileRecord::XML_RECORD = "record";
const QString XmlFileRecord::XML_FILE = "file";
const QString XmlFileRecord::XML_TIME = "time";
const QString XmlFileRecord::XML_FLAGS = "flags";
const QString XmlFileRecord::XML_VALID= "valid"; 
const QString XmlFileRecord::XML_PROPERTY= "property"; 
const QString XmlFileRecord::XML_NAME= "name"; 
const QString XmlFileRecord::XML_VALUE= "value"; 
//@}

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
    if( attribute.name() == XML_FILE ) setFile( qPrintable( XmlString( attribute.value() ).toText() ) );
    else if( attribute.name() == XML_TIME ) setTime( attribute.value().toInt() );
    else if( attribute.name() == XML_FLAGS ) setFlags( attribute.value().toUInt() );
    else if( attribute.name() == XML_VALID ) setValid( attribute.value().toInt() );
    else addProperty( qPrintable( attribute.name() ), qPrintable( attribute.value() ) );
  }

  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    if( child_element.isNull() ) continue;

    QString tag_name( child_element.tagName() );
    if( tag_name == XML_PROPERTY ) 
    {

      std::pair< QString, QString > property;
      
      // load attributes
      QDomNamedNodeMap attributes( child_element.attributes() );
      for( unsigned int i=0; i<attributes.length(); i++ )
      {
        
        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() || attribute.name().isEmpty() ) continue;
        
        if( attribute.name() == XML_NAME ) property.first = XmlString( attribute.value() ).toText();
        else if( attribute.name() == XML_VALUE ) property.second = XmlString( attribute.value() ).toText();
        else cout << "XmlFileRecord::XmlFileRecord - unrecognized attribute " << qPrintable( attribute.name() ) << endl;
        
      }
      
      if( !( property.first.isEmpty() || property.second.isEmpty() ) )
      { addProperty( qPrintable( property.first ), qPrintable( property.second ) ); }
      
    } else cout << "XmlFileRecord::XmlFileRecord - unrecognized child " << qPrintable( child_element.tagName() ) << ".\n";
  }
  
}

//_______________________________________________
QDomElement XmlFileRecord::domElement( QDomDocument& parent ) const
{
  Debug::Throw( "XmlFileRecord::domElement.\n" );
  QDomElement out( parent.createElement( XML_RECORD ) );
  out.setAttribute( XML_FILE, XmlString( file().c_str() ).toXml() );
  out.setAttribute( XML_TIME, Str().assign<int>( XmlFileRecord::time() ).c_str() );
  out.setAttribute( XML_VALID, Str().assign<bool>( isValid() ).c_str() );

  if( flags() ) out.setAttribute( XML_FLAGS, Str().assign<unsigned int>( flags() ).c_str() );
  
  // for( PropertyMap::const_iterator iter = properties().begin(); iter != properties().end(); iter++ )
  // { out.setAttribute( PropertyId::get(iter->first).c_str(), iter->second.c_str() ); }
  
  for( PropertyMap::const_iterator iter = properties().begin(); iter != properties().end(); iter++ )
  {
    QDomElement property( parent.createElement( XML_PROPERTY ) );
    property.setAttribute( XML_NAME, XmlString( PropertyId::get(iter->first).c_str() ).toXml() );
    property.setAttribute( XML_VALUE, XmlString( iter->second.c_str() ) );
    out.appendChild( property );
  }
  
  return out;
}

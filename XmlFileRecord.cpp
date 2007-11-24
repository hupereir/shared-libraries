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
#include "XmlUtil.h" 

using namespace std;

//!@name XML names
//@{
const string XmlFileRecord::XML_RECORD = "record";
const string XmlFileRecord::XML_FILE = "file";
const string XmlFileRecord::XML_TIME = "time";
const string XmlFileRecord::XML_VALID= "valid"; 
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
    if( attribute.isNull() ) continue;
    Str name( qPrintable( attribute.name() ) );
    Str value( qPrintable( attribute.value() ) );
    if( name == XML_FILE ) setFile( XmlUtil::xmlToText( value ) );
    else if( name == XML_TIME ) setTime( value.get<int>() );
    else if( name == XML_VALID ) setValid( value.get<bool>() );
    else addInformation( name, value );
  }
}

//_______________________________________________
QDomElement XmlFileRecord::domElement( QDomDocument& parent ) const
{
  Debug::Throw( "XmlFileRecord::domElement.\n" );
  QDomElement out( parent.createElement( XML_RECORD.c_str() ) );
  out.setAttribute( XML_FILE.c_str(), XmlUtil::textToXml( file() ).c_str() );
  out.setAttribute( XML_TIME.c_str(), Str().assign<int>( time() ).c_str() );
  out.setAttribute( XML_VALID.c_str(), Str().assign<bool>( isValid() ).c_str() );
  
  for( InfoMap::const_iterator iter = _informations().begin(); iter != _informations().end(); iter++ )
  { out.setAttribute( iter->first.c_str(), iter->second.c_str() ); }
  
  return out;
}

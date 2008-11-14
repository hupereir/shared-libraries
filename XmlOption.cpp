
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
   \file XmlOption.cpp
   \brief Xml implementation of the Option object
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Str.h"
#include "XmlOption.h"
#include "XmlString.h"
using namespace std;

//________________________________________________
XmlOption::XmlOption( const QDomElement& element )
{
  Debug::Throw( "XmlOption::XmlOption.\n" );
  setName( qPrintable( element.nodeName() ) );
  
  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    if( attribute.name() == OPTIONS::VALUE ) setRaw( qPrintable( XmlString( attribute.value() ).toText() ) );
    else if( attribute.name() == OPTIONS::COMMENTS ) setComments( qPrintable( XmlString( attribute.value() ).toText() ) );
    else if( attribute.name() == OPTIONS::FLAGS ) {
      
      setFlags( (unsigned int) attribute.value().toInt() );
    
    } else cout << "XmlOption::XmlOption - unrecognized attribute " << qPrintable( attribute.name() ) << ".\n";
    
  }

  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    if( child_element.tagName() == OPTIONS::COMMENTS ) setComments( qPrintable( XmlString( child_element.text() ).toText() ) );
    else if( child_element.tagName() == OPTIONS::VALUE ) setRaw( qPrintable( XmlString( child_element.text() ).toText() ) );  
    else if( child_element.tagName() == OPTIONS::FLAGS ) setFlags( (unsigned int) child_element.text().toInt() );
    else cout << "XmlOption::XmlOption - unrecognized child " << qPrintable( child_element.tagName() ) << ".\n";

  }

}

//________________________________________________
QDomElement XmlOption::domElement( QDomDocument& document ) const
{

  Debug::Throw() << "XmlOption::DomElement - " << name() << " - " << raw() << endl;
  
  QDomElement out = document.createElement( name().c_str() );
  
  out.
    appendChild( document.createElement( OPTIONS::VALUE ) ).
    appendChild( document.createTextNode( XmlString( raw().c_str() ).toXml() ) );

  out.
    appendChild( document.createElement( OPTIONS::FLAGS ) ).
    appendChild( document.createTextNode( QString().setNum( flags() ) ) );

  if( comments().size() )
  {
    out.
      appendChild( document.createElement( OPTIONS::COMMENTS ) ).
      appendChild( document.createTextNode( XmlString( comments().c_str() ).toXml() ) );
  }
   
  return out;

}

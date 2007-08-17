
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
   \file XmlOption.cc
   \brief Xml implementation of the Option object
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Str.h"
#include "XmlOption.h"
#include "XmlUtil.h"
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
    string name( qPrintable( attribute.name() ) );
    string value( qPrintable( attribute.value() ) );

    if( name == OPTIONS::VALUE ) setRaw( XmlUtil::xmlToText(value) );
    else if( name == OPTIONS::COMMENTS ) setComments( XmlUtil::xmlToText(value) );
    else if( name == OPTIONS::OPTIONS )
    {
      if( Str( OPTIONS::FRONT ).isIn( value, false ) ) setFront( true );
    } else cout << "XmlOption::XmlOption - unrecognized attribute " << name << ".\n";

  }

  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    string tag_name( qPrintable( child_element.tagName() ) );
    if( tag_name == OPTIONS::COMMENTS )
    setComments( XmlUtil::xmlToText( qPrintable( child_element.text() ) ) );
    else cout << "XmlOption::XmlOption - unrecognized child " << qPrintable( child_element.tagName() ) << ".\n";
  }

  _setValid( raw().size() );

}

//________________________________________________
QDomElement XmlOption::domElement( QDomDocument& parent ) const
{

  Debug::Throw() << "XmlOption::DomElement - " << name() << " - " << raw() << endl;
  
  QDomElement out = parent.createElement( name().c_str() );
  out.setAttribute( OPTIONS::VALUE.c_str(), XmlUtil::textToXml( raw() ).c_str() );

  if( front() ) out.setAttribute( OPTIONS::OPTIONS.c_str(), OPTIONS::FRONT.c_str() );
  if( comments().size() )
  out.
    appendChild( parent.createElement( OPTIONS::COMMENTS.c_str() ) ).
    appendChild( parent.createTextNode( XmlUtil::textToXml( comments() ).c_str() ) );

  return out;

}

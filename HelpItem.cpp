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

/*!
   \file    HelpItem.cpp
   \brief   reference manual item. 
            it contains a title and a text
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <iostream>

#include "HelpItem.h"
#include "XmlString.h"

using namespace std;
using namespace BASE;

//_________________________________________________________
HelpItem::HelpItem( const QDomElement& element ):
  Counter( "HelpItem" )
{

  Debug::Throw( "HelpItem::HelpItem.\n" );

  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;

    if( attribute.name() == XML_LABEL ) label_ = XmlString( attribute.value() ).toText();
    else Debug::Throw() << "HelpItem::HelpItem - unrecognized attribute: " << qPrintable( attribute.name() ) << endl;
    
  }

  // parse children
  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    if( child_element.tagName() == XML_TEXT ) text_ = XmlString( child_element.text() ).toText();
    else cout << "HelpItem::HelpItem - unrecognized child " << qPrintable( child_element.tagName() ) << endl;
  }

}

//_________________________________________________________
QDomElement HelpItem::domElement( QDomDocument& parent ) const
{
  Debug::Throw( "HelpItem::DomElement.\n" );
  QDomElement out = parent.createElement( XML_ITEM );
  out.setAttribute( XML_LABEL, XmlString( label_ ).toXml() );

  // text child
  if( text_.size() )
  out.
    appendChild( parent.createElement( XML_TEXT ) ).
    appendChild( parent.createTextNode( XmlString( text_ ).toXml() ) );

  return out;

}

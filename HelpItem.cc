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
   \file    HelpItem.cc
   \brief   reference manual item. 
            it contains a title and a text
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <iostream>

#include "HelpItem.h"
#include "XmlUtil.h"

using namespace std;
using namespace HELP;

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
    string name( qPrintable( attribute.name() ) );
    string value( qPrintable( attribute.value() ) );

    if( name == XML_LABEL ) label_ = XmlUtil::xmlToText(value);
    else Debug::Throw() << "HelpItem::HelpItem - unrecognized attribute: " << name << endl;
  }

  // parse children
  // parse children elements
  for(QDomNode child_node = element.firstChild(); !child_node.isNull(); child_node = child_node.nextSibling() )
  {
    QDomElement child_element = child_node.toElement();
    string tag_name( qPrintable( child_element.tagName() ) );
    if( tag_name == XML_TEXT ) text_ = XmlUtil::xmlToText( qPrintable( child_element.text() ) );
    else cout << "HelpItem::HelpItem - unrecognized child " << qPrintable( child_element.tagName() ) << endl;
  }

}

//_________________________________________________________
QDomElement HelpItem::domElement( QDomDocument& parent ) const
{
  Debug::Throw( "HelpItem::DomElement.\n" );
  QDomElement out = parent.createElement( XML_ITEM.c_str() );
  out.setAttribute( XML_LABEL.c_str(), XmlUtil::textToXml( label_ ).c_str() );

  // text child
  if( text_.size() )
  out.
    appendChild( parent.createElement( XML_TEXT.c_str() ) ).
    appendChild( parent.createTextNode( XmlUtil::textToXml( text_ ).c_str() ) );

  return out;

}

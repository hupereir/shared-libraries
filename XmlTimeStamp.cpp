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
   \file    XmlTimeStamp.cpp
   \brief   Xml interface to time manipulation object
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "XmlTimeStamp.h"

using namespace std;

const QString XmlTimeStamp::XML_TIME( "time" );

//______________________________________________________
XmlTimeStamp::XmlTimeStamp( const QDomElement& element )
{
  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    if( attribute.name() == XML_TIME ) setTime( attribute.value().toUInt() );
  }
};

//_______________________________________________________
QDomElement XmlTimeStamp::domElement( const QString& name, QDomDocument& parent ) const
{
  QDomElement out( parent.createElement( name ) );
  out.setAttribute( XML_TIME, QString().setNum( unixTime() ) );
  return out;  
}

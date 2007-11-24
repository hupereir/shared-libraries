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
  \file XmlTextFormatBlock.cpp
  \brief tracks text format in given text range
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "Str.h"
#include "XmlTextFormatBlock.h"

using namespace std;
using namespace FORMAT;

//____________________________________
const string XmlTextFormatBlock::XML_TAG = "TextFormat";
const string XmlTextFormatBlock::XML_FORMAT = "format";
const string XmlTextFormatBlock::XML_COLOR = "color";

const string XmlTextFormatBlock::XML_BEGIN = "begin";
const string XmlTextFormatBlock::XML_END = "end";

// obsolete tag names
const string XmlTextFormatBlock::XML_BEGIN_PAR = "begin_par";
const string XmlTextFormatBlock::XML_BEGIN_INDEX = "begin_index";
const string XmlTextFormatBlock::XML_END_PAR = "end_par";
const string XmlTextFormatBlock::XML_END_INDEX = "end_index";
  
//____________________________________
XmlTextFormatBlock::XmlTextFormatBlock( const QDomElement& element )
{
    
  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    Str name( qPrintable( attribute.name() ) );
    Str value( qPrintable( attribute.value() ) );

    // nominal tags
    if( name == XML_BEGIN ) begin() = value.get<unsigned int>();
    else if( name == XML_END ) end() = value.get<unsigned int>();
    
    // obsolete paragraph tags
    else if( name == XML_BEGIN_PAR ) _parBegin() = Str( value ).get<int>();
    else if( name == XML_END_PAR ) _parEnd() = value.get<int>();
    
    // obsolete index tags
    else if( name == XML_BEGIN_INDEX ) begin() = value.get<unsigned int>();
    else if( name == XML_END_INDEX ) end() = value.get<unsigned int>();
    
    // format
    else if( name == XML_FORMAT ) format() =value.get<unsigned int>();
    else if( name == XML_COLOR ) color() = value;
    
    else cerr << "XmlTextFormatBlock::XmlTextFormatBlock - unrecognized timestamp attribute: \"" << name << "\"\n";
  }

}

//__________________________________________________________
QDomElement XmlTextFormatBlock::domElement( QDomDocument& parent ) const
{
  QDomElement out( parent.createElement( XML_TAG.c_str() ) );
  out.setAttribute( XML_BEGIN.c_str(), Str().assign<int>(begin()).c_str() );  
  out.setAttribute( XML_END.c_str(), Str().assign<int>(end()).c_str() );  
  out.setAttribute( XML_FORMAT.c_str(), Str().assign<unsigned int>(format()).c_str() );
  out.setAttribute( XML_COLOR.c_str(), color().c_str() );
  return out;  
}

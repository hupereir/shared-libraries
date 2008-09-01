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
   \file XmlString.cpp
   \brief   Some Xml utilities
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Debug.h"
#include "XmlString.h"
#include "Str.h"
#include "iostream"

using namespace std;

//__________________________________
XmlString::ConversionList XmlString::conversions_;
  
//__________________________________
void XmlString::_initConversions() const
{
  
  conversions_.push_back( Conversion( "<", "XML_LT" ) );
  conversions_.push_back( Conversion( ">", "XML_GT" ) );
  conversions_.push_back( Conversion( "&", "XML_AND" ) );
  conversions_.push_back( Conversion( "\"", "XML_QUOTE" ) );
  conversions_.push_back( Conversion( "�", "XML_AGRAVE" ) );
  conversions_.push_back( Conversion( "�", "XML_AHAT" ) );
  conversions_.push_back( Conversion( "�", "XML_ECUTE" ) );
  conversions_.push_back( Conversion( "�", "XML_EGRAVE" ) );
  conversions_.push_back( Conversion( "�", "XML_EHAT" ) );
  conversions_.push_back( Conversion( "�", "XML_IHAT" ) );
  conversions_.push_back( Conversion( "�", "XML_OHAT" ) );
  conversions_.push_back( Conversion( "�", "XML_UGRAVE" ) );
  conversions_.push_back( Conversion( "�", "XML_CCED" ) );
  conversions_.push_back( Conversion( "\t", "XML_TAB" ) );
  conversions_.push_back( Conversion( "\n", "XML_ENDL" ) );
  
  // this conversion is needed for XML not to remove entries that consist of empty spaces only
  // it is used in xmlToText but not in textToXml
  conversions_.push_back( Conversion( "", "XML_NONE" ) );
  
  return;
  
}

//__________________________________
QString XmlString::toXml( void ) const
{

  QString out(*this);
  if( !( conversions_.size() ) ) _initConversions();
  for( ConversionList::iterator iter = conversions_.begin(); iter != conversions_.end(); iter++ )
  { if( !iter->first.isEmpty() ) out = out.replace( iter->first, iter->second ); }
  
  return out;
}

//__________________________________
QString XmlString::toText( void ) const
{

  QString out(*this);
  if( !( conversions_.size() ) ) _initConversions();

  // HTML style conversions (escape characters)
  for( ConversionList::reverse_iterator iter = conversions_.rbegin(); iter != conversions_.rend(); iter++ )
  { if( !iter->second.isEmpty() ) out = out.replace( iter->second, iter->first ); }

  return out;
}

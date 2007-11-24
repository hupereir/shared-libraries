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
   \file XmlUtil.cpp
   \brief   Some Xml utilities
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Debug.h"
#include "XmlUtil.h"
#include "Str.h"
#include "iostream"

using namespace std;

//__________________________________
XmlUtil::ConversionList XmlUtil::conversions_;
bool XmlUtil::initialized_ = _initConversions();
  
//__________________________________
bool XmlUtil::_initConversions()
{
  
  conversions_.clear();
  conversions_.push_back( Conversion( "<", "XML_LT" ) );
  conversions_.push_back( Conversion( ">", "XML_GT" ) );
  conversions_.push_back( Conversion( "&", "XML_AND" ) );
  conversions_.push_back( Conversion( "\"", "XML_QUOTE" ) );
  conversions_.push_back( Conversion( "à", "XML_AGRAVE" ) );
  conversions_.push_back( Conversion( "â", "XML_AHAT" ) );
  conversions_.push_back( Conversion( "é", "XML_ECUTE" ) );
  conversions_.push_back( Conversion( "è", "XML_EGRAVE" ) );
  conversions_.push_back( Conversion( "ê", "XML_EHAT" ) );
  conversions_.push_back( Conversion( "î", "XML_IHAT" ) );
  conversions_.push_back( Conversion( "ô", "XML_OHAT" ) );
  conversions_.push_back( Conversion( "ù", "XML_UGRAVE" ) );
  conversions_.push_back( Conversion( "ç", "XML_CCED" ) );
  conversions_.push_back( Conversion( "\t", "XML_TAB" ) );
  conversions_.push_back( Conversion( "\n", "XML_ENDL" ) );
  return true;
  
}

//__________________________________
string XmlUtil::textToXml( const string& in )
{
  Str out( in );
  if( !( conversions_.size() ) ) _initConversions();
  for( ConversionList::iterator iter = conversions_.begin(); iter != conversions_.end(); iter++ )
  out = out.replace( iter->first, iter->second );
  
  return out;
}

//__________________________________
string XmlUtil::xmlToText( const string& in )
{
  Str out( in );
  if( !( conversions_.size() ) ) _initConversions();

  // HTML style conversions (escape characters)
  for( ConversionList::reverse_iterator iter = conversions_.rbegin(); iter != conversions_.rend(); iter++ )
  { out = out.replace( iter->second, iter->first ); }

  return out;
}

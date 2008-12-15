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
XmlString::ConversionList& XmlString::_conversions( void )
{
  static ConversionList conversions;
  return conversions;
}

//__________________________________
void XmlString::_initConversions() const
{
  
  _conversions().push_back( Conversion( "<", "XML_LT" ) );
  _conversions().push_back( Conversion( ">", "XML_GT" ) );
  _conversions().push_back( Conversion( "&", "XML_AND" ) );
  _conversions().push_back( Conversion( "\"", "XML_QUOTE" ) );
  _conversions().push_back( Conversion( "à", "XML_AGRAVE" ) );
  _conversions().push_back( Conversion( "â", "XML_AHAT" ) );
  _conversions().push_back( Conversion( "é", "XML_ECUTE" ) );
  _conversions().push_back( Conversion( "è", "XML_EGRAVE" ) );
  _conversions().push_back( Conversion( "ê", "XML_EHAT" ) );
  _conversions().push_back( Conversion( "î", "XML_IHAT" ) );
  _conversions().push_back( Conversion( "ô", "XML_OHAT" ) );
  _conversions().push_back( Conversion( "ù", "XML_UGRAVE" ) );
  _conversions().push_back( Conversion( "ç", "XML_CCED" ) );
  _conversions().push_back( Conversion( "\t", "XML_TAB" ) );
  _conversions().push_back( Conversion( "\n", "XML_ENDL" ) );
  
  // this conversion is needed for XML not to remove entries that consist of empty spaces only
  // it is used in xmlToText but not in textToXml
  _conversions().push_back( Conversion( "", "XML_NONE" ) );
  
  return;
  
}

//__________________________________
QString XmlString::toXml( void ) const
{

  QString out(*this);
  if( !( _conversions().size() ) ) _initConversions();
  for( ConversionList::iterator iter = _conversions().begin(); iter != _conversions().end(); iter++ )
  { if( !iter->first.isEmpty() ) out = out.replace( iter->first, iter->second ); }
  
  return out;
}

//__________________________________
QString XmlString::toText( void ) const
{

  QString out(*this);
  if( !( _conversions().size() ) ) _initConversions();

  // HTML style conversions (escape characters)
  for( ConversionList::reverse_iterator iter = _conversions().rbegin(); iter != _conversions().rend(); iter++ )
  { if( !iter->second.isEmpty() ) out = out.replace( iter->second, iter->first ); }

  return out;
}

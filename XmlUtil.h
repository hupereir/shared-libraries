#ifndef XmlUtil_h
#define XmlUtil_h

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
   \file XmlUtil.h
   \brief Some Xml utilities
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/
#include <string>
#include <list>

/*!
   \class XmlUtil
   \brief Some Xml utilities
*/

class XmlUtil {

  public:
  
  //! convert input string from text to xml
  static std::string textToXml( const std::string& in ); 
  
  //! convert input string from xml to text
  static std::string xmlToText( const std::string& in ); 
  
  //! parser status
  enum Status {
    
    //! parser is invalid
    INVALID,  
        
    //! parser is being read
    STARTED,  
        
    //! parsing is done
    COMPLETED 
  };
    
  private:
  
  //! initialize Xml to text conversion pair list
  static bool _initConversions( void );
  
  //! text to Xml conversion pair type
  typedef std::pair<std::string, std::string> Conversion;
  
  //! text to Xml conversion pair type
  typedef std::list< Conversion > ConversionList;
  
  //! text to Xml conversion pair list
  static ConversionList conversions_;
 
  //! make sure initialization is done once at startup
  static bool initialized_;
  
};

#endif

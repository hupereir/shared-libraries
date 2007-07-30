#ifndef XmlOption_h
#define XmlOption_h

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
   \file XmlOption.h
   \brief Xml implementation of the Option object
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QDomDocument>
#include <QDomElement>

#include "Option.h"

//! some XML definitions specific to Option management
namespace OPTIONS {

  /*! 
    \brief Xml special options tag.
    special meaning that option name do not have to be unique
  */
  static const std::string SPECIAL_OPTION = "SPECIAL_OPTION";

  //! Xml option tag
  static const std::string OPTIONS = "Options";

  //! Xml string for value field
  static const std::string VALUE = "Value";

  //! Xml string for comments field
  static const std::string COMMENTS = "Comments";

  //! Xml string for FRONT added special options
  static const std::string FRONT = "FRONT";
    
};

//! Xml implementation of the Option object
class XmlOption:public Option
{
  
  public:

  //! constructor
  XmlOption( const Option& option ):
    Option( option )
    {}
  
  //! creator from DOM node
  XmlOption( const QDomElement& element );

  //! retrieve DomElement from option
  QDomElement domElement( QDomDocument& parent ) const;
  
};

#endif

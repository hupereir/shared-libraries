// $Id$
#ifndef XmlTextFormatBlock_h
#define XmlTextFormatBlock_h

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
  \file XmlTextFormatBlock.h
  \brief text format with Xml abilities
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QDomElement>
#include <QDomDocument>
#include "TextFormatBlock.h"

//! text format with Xml abilities
namespace FORMAT
{
  class XmlTextFormatBlock:public TextFormatBlock
  {
    
    public:
    
    //! constructor
    XmlTextFormatBlock( const QDomElement& element );
    
    //! constructor
    XmlTextFormatBlock( const TextFormatBlock& format ):
      TextFormatBlock( format )
      {}  
  
    //! get dom element
    QDomElement domElement( QDomDocument& parent ) const;
      
    //!@name XML_tag names
    //@{
    
    static const QString XML_TAG;
    static const QString XML_BEGIN;
    static const QString XML_END;
    static const QString XML_FORMAT;
    static const QString XML_COLOR;
    
    //@}
    
    //!@name old tags, kept for backward compatibility
    //@{
    
    static const QString XML_BEGIN_PAR;
    static const QString XML_BEGIN_INDEX;
    static const QString XML_END_PAR;
    static const QString XML_END_INDEX;
    
    //@}
    
  };
  
};

#endif

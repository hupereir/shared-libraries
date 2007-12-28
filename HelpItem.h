#ifndef HelpItem_h
#define HelpItem_h

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
   \file    HelpItem.h
   \brief   reference manual item. 
            it contains a title and a text
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QDomElement>
#include <QDomDocument>
#include <string>
#include <vector>

#include "Counter.h"
#include "Debug.h"

//! Help system namespace
namespace BASE
{

  
  //!@name xml tags
  //@{
  const std::string XML_HELP = "help";
  const std::string XML_ITEM = "item";
  const std::string XML_LABEL = "label";
  const std::string XML_TEXT = "text";
  //@}
  
  //! reference manual single entry 
  class HelpItem : public Counter
  {
    
    public:
    
    //! constructor
    HelpItem( const std::string& label = "", const std::string& text = "" ):
      Counter( "HelpItem" ),
      label_( label ),
      text_( text )
    { Debug::Throw( "HelpItem::HelpItem.\n" ); }
    
    //! constructor from Dom
    HelpItem( const QDomElement& dom );
    
    //! write to dom elelement
    QDomElement domElement( QDomDocument& parent ) const;
    
    //! equal-to operator
    bool operator == (const HelpItem& item ) const
    { return label() == item.label(); }

    //! equal-to operator
    bool operator < (const HelpItem& item ) const
    { return label() < item.label(); }

    //! item label
    void setLabel( const std::string& label ) 
    { label_ = label; }
    
    //! label
    const std::string& label( void ) const
    { return label_; }
    
    //! item text
    void setText( const std::string& text ) 
    { text_ = text; }
    
    //! item text
    const std::string& text( void ) const
    { return text_; }
    
    typedef std::vector< HelpItem > List;
    
    private:
    
    //! help label
    std::string label_;
    
    //! help text
    std::string text_;
    
  };

};

#endif

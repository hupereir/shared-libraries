#ifndef TextSelection_h
#define TextSelection_h

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
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/
 
/*!
  \file TextSelection.h
  \brief find_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/  

#include <QString>
#include <string>
#include <iostream>

#include "Debug.h"

//! string to be found and options
class TextSelection
{
  public:
  
  //! default constructor
  TextSelection( 
    const QString& text = "",
    const QString& replace_text = "",
    const unsigned int& flags = NONE ):
    text_( text ),
    replace_text_( replace_text ),
    flags_( flags )
  {}
  
  //! text selection flags
  enum Flag
  {
    //! no flags
    NONE = 0,
        
    //! search backward
    BACKWARD = 1<<0,
        
    //! case sensitive search
    CASE_SENSITIVE = 1<<1,
    
    //! entire word search
    ENTIRE_WORD = 1<<2,
    
    //! regular expressions search
    REGEXP = 1<<3,
    
    //! incremental search
    NO_INCREMENT = 1<<4
  };
  
  //! text
  const QString& text( void ) const
  { return text_; }
  
  //! test
  void setText( const QString& text ) 
  { text_ = text; }
  
  //! replace text
  const QString& replaceText( void ) const
  { return replace_text_; }
  
  //! replace text
  void setReplaceText( const QString& text )
  { replace_text_ = text; }
  
  //! flags
  const unsigned int& flags( void ) const
  { return flags_; }
  
  //! flags
  void setFlags( const unsigned int& flags )
  { flags_ = flags; }

  //! flags
  bool flag( const Flag& flag ) const
  { return flags_ & flag; }
  
  //! flags
  void setFlag( const Flag& flag, const bool& value )
  { 
    if( value ) flags_ |= flag; 
    else flags_ &= (~flag);
  }
  
  private:
            
  //! string to be found
  QString text_;
            
  //! string to replace
  QString replace_text_;
  
  //! flags
  unsigned int flags_;
    
  //! streamer
  friend std::ostream &operator << (std::ostream &out,const TextSelection &selection)
  {
    out 
        << "TextSelection -"
        << " text: " << qPrintable(selection.text()) 
        << " replacement: " << qPrintable(selection.replaceText())
        << " backward: " << (selection.flag(BACKWARD) ? "true":"false")
        << " case_sensitive: " << (selection.flag(CASE_SENSITIVE) ? "true":"false")
        << " entire_word: " << (selection.flag(ENTIRE_WORD) ? "true":"false")        
        << " regex: " << (selection.flag(REGEXP) ? "true":"false")
        << " no_increment: " << (selection.flag(NO_INCREMENT) ? "true":"false");
    return out;
  }
  
};

#endif

#ifndef TextBlockData_h
#define TextBlockData_h

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
  \file TextBlockData.h
  \brief TextBlock data
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/  

#include <QTextBlockUserData>
#include "TextBlockFlags.h"
#include "Counter.h"

//! TextBlock data
class TextBlockData: public QTextBlockUserData, public Counter
{
  
  public: 
  
  //! constructor
  TextBlockData():
    QTextBlockUserData(),
    Counter( "TextBlockData" ),
    flags_( TextBlock::NONE )
  { Debug::Throw( "TextBlockData::TextBlockData.\n" ); }
  
  //! destructor
  virtual ~TextBlockData( void )
  { Debug::Throw( "TextBlockData::~TextBlockData.\n" ); }
  
   //! flags
  const unsigned int& flags( void ) const
  { return flags_; }
  
  //! flags
  void setFlags( const unsigned int& flags )
  { flags_ = flags; }

  //! flags
  bool hasFlag( const unsigned int& flag ) const
  { return flags_ & flag; }
  
  //! flags
  void setFlag( const unsigned int& flag, const bool& value )
  { 
    if( value ) flags_ |= flag; 
    else flags_ &= (~flag);
  } 
  
  //! block background
  /*! returns true if changed */
  bool setBackground( const QColor& color ) 
  { 
    if( (background_.isValid() || color.isValid() ) && color != background_ ) 
    {
      background_ = color; 
      setFlag( TextBlock::HAS_BACKGROUND, color.isValid() );
      return true;
    } else return false;
  }

  //! block background
  const QColor& background( void ) const
  { return background_; }
  
  private:
  
  //! flags
  /* is a bit pattern */
  unsigned int flags_;
  
  //! set to true for current block
  bool current_block_;
    
  //! block background color available
  bool has_background_;
  
  //! block background color (overridden by active)
  QColor background_;
  
};

#endif

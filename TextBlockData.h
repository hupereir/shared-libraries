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
#include "Counter.h"

//! TextBlock data
class TextBlockData: public QTextBlockUserData, public Counter
{
  
  public: 
  
  //! constructor
  TextBlockData():
    QTextBlockUserData(),
    Counter( "TextBlockData" ),
    current_block_( false ),
    has_background_( false )
  { Debug::Throw( "TextBlockData::TextBlockData.\n" ); }
  
  //! destructor
  virtual ~TextBlockData( void )
  { Debug::Throw( "TextBlockData::~TextBlockData.\n" ); }

  //! true if background is available/valid
  const bool& hasBackground( void ) const
  { return has_background_; }
  
  //! block background
  const QColor& background( void ) const
  { return background_; }
  
  //! block background
  /*! returns true if changed */
  bool setBackground( const QColor& color ) 
  { 
    if( !(background_.isValid() || color.isValid() ) || color == background_ ) return false;
    background_ = color; 
    has_background_ = color.isValid(); 
    return true;
  }
  
  //! active block
  const bool& isCurrentBlock( void ) const
  { return current_block_; }
  
  //! active block
  void setIsCurrentBlock( const bool& value )
  { current_block_ = value; }
  
  private:
  
  //! set to true for current block
  bool current_block_;
    
  //! block background color available
  bool has_background_;
  
  //! block background color (overridden by active)
  QColor background_;
  
};

#endif

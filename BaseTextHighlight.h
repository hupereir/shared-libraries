#ifndef _BaseTextHighlight_h_
#define _BaseTextHighlight_h_

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
  \file BaseTextHighlight.h
  \brief current paragraph syntax highlighter
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QSyntaxHighlighter>

#include "Counter.h"
#include "Debug.h"
#include "Key.h"

//! syntax highlighting based on text patterns
class BaseTextHighlight: public QSyntaxHighlighter, public BASE::Key, public Counter
{
  
  public:
  
  //! constructor
  BaseTextHighlight( QTextDocument* );
  
  //! highlight paragraph
  virtual void highlightBlock( const QString& text );
    
  //! enable highlight. Returns true if changed
  bool setEnabled( const bool& state )
  { 
    if( enabled_ == state ) return false;
    enabled_ = state; 
    return true;
  }
  
  //! true if enabled
  const bool& isEnabled( void ) const
  { return enabled_; }
  
  //! highlight color
  void setHighlightColor( const QColor& color )
  { color_ = color; }
  
  //! retrieve highlight color
  const QColor& highlightColor( void ) const
  { return color_; }
  
  private:
  
  //! true if highlight is enabled
  bool enabled_;

  //! highlight color
  QColor color_;
  
};

#endif

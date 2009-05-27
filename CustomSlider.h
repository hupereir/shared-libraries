// $Id$
#ifndef _CustomSlider_h_
#define _CustomSlider_h_


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
\file CustomSlider.h
\brief customized QSlider associated to a LineEditor
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cassert>
#include <QSlider>

#include "AnimatedLineEditor.h"
#include "Counter.h"  
#include "Debug.h"  
/*!
\class CustomSlider
\brief customized QSlider associated to a LineEditor
*/
class CustomSlider: public QWidget, public Counter
{
  
  //! Qt meta object declaration
  Q_OBJECT
    
  public:
    
  //! creator
  CustomSlider( QWidget* parent );
  
  //! destructor
  ~CustomSlider( void ) 
  {}
  
  //! retrieve QSlider
  QSlider& slider( void )
  {
    assert( slider_ );
    return *slider_;
  }
  
  //! retrieve QSlider
  const QSlider& slider( void ) const
  {
    assert( slider_ );
    return *slider_;
  }
  
  //! retrieve LineEditor
  AnimatedLineEditor& lineEditor( void )
  {
    assert( line_edit_ );
    return *line_edit_;
  }
  
  //! changes value
  void setValue( int value );
  
  //! retrieves value
  int value( void ) const
  { return slider_->value(); }

  private:
  
  //! associated slider
  QSlider *slider_;
  
  //! associated LineEdit
  AnimatedLineEditor *line_edit_;
  
  private slots:
  
  //! changes QSlider value according to LineEditor
  void _updateSlider( void );
  
  //! changes LineEditor value according to QSlider
  void _updateLineEdit( int );
  
};

#endif

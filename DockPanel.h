#ifndef _DockPanel_h_
#define _DockPanel_h_

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
  \file DockPanel.h
  \brief detachable generic panel
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

#include <string>

#include "Counter.h"

//! detachable generic panel
class DockPanel: public QWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! dock panel flags  
  enum Flags
  {
    
    //! no flag
    NONE = 0,
        
    //! dock panel stays on top of other windows
    STAYS_ON_TOP = 1
    
  };
      
  //! constructor
  DockPanel( QWidget* parent, const unsigned int& flags = STAYS_ON_TOP );

  //! destructor
  virtual ~DockPanel()
  {}
  
  //! dock panel flags
  void setFlags( const unsigned int& flags )
  { flags_ = flags; }
  
  //! get dock main widget
  virtual QFrame& mainWidget( void )
  {
    Exception::checkPointer( main_, DESCRIPTION( "main_ not initialized" ) );
    return *main_;
  }
  
  //! get box (to add contents)
  virtual QWidget& getBox( void )
  { 
    Exception::checkPointer( box_, DESCRIPTION( "box_ not initialized" ) );
    return *box_;
  }
    
  //! set detachable group box title
  void setTitle( const std::string& title )
  { 
    if( !label_ ) {
      label_ = new QLabel( main_ );
      QFont font( label_->font() );
      font.setBold( true );
      label_->setFont( font );
      main_layout_->insertWidget( 0, label_ );
    }
    label_->setText( title.c_str() );
  }
  
  //! retrieve button layout
  QBoxLayout& buttonLayout( void ) const
  {
    Exception::checkPointer( button_layout_, DESCRIPTION( "button_layout_ not initialized" ) );
    return *button_layout_;
  }
          
  signals:
  
  //! emmited when box is detached
  void detached( void );
  
  //! emmited when box is attached
  void attached( void );
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  private:
  
  //! flags
  unsigned int flags_;
    
  //! vertical layout for main_ widget
  QVBoxLayout* main_layout_;
  
  //! horizontal layout (for buttons)
  QHBoxLayout* button_layout_;
      
  //! title label
  QLabel* label_;
      
  //! button
  QPushButton* button_;
  
  //! detachable main widget
  QFrame* main_;
    
  //! contents vbox
  QWidget* box_;
  
  //! default size for the detached panel
  QSize detached_size_;
  
};

#endif

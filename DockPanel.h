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

#include <QCloseEvent>
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
  
  //! get panel (to add contents)
  virtual QWidget& panel( void )
  { 
    Exception::checkPointer( panel_, DESCRIPTION( "panel_ not initialized" ) );
    return *panel_;
  }
    
  //! set detachable group panel title
  void setTitle( const std::string& title )
  { 
    if( !label_ ) {
      label_ = new QLabel( main_ );
      QFont font( label_->font() );
      font.setWeight( QFont::Bold );
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
  
  //! emmited when state is changed
  void attached( bool state );
  
  //! emmited when panel is detached
  void detached( void );
  
  //! emmited when panel is attached
  void attached( void );
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  private:
  
  //! local widget to implement close_event of the content
  class LocalWidget: public QFrame, public Counter
  {
    public:
    
    //! constructor
    LocalWidget( DockPanel* parent ):
      QFrame( parent ),
      Counter( "DockPanel::LocalWidget" ),
      panel_( parent )
    {}
    
    protected:
    
    //! closeEvent
    virtual void closeEvent( QCloseEvent* event )
    {
      Debug::Throw( "DockPanel::LocalWidget::closeEvent.\n" );
      if( !parent() ) panel_->_toggleDock();
      event->ignore();
    }
    
    private:
    
    //! parent panel
    DockPanel *panel_;
    
  };
  
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
  LocalWidget* main_;
    
  //! contents panel
  QWidget* panel_;
  
  //! default size for the detached panel
  QSize detached_size_;
  
  friend class LocalWidget;
  
};

#endif

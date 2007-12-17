#ifndef _CustomTabWidget_h_
#define _CustomTabWidget_h_

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
  \file CustomTabWidget.h
  \brief Tab widget with detachable pages
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QCloseEvent>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSizeGrip>
#include <QTabWidget>
#include <string>

#include "Counter.h"
#include "Debug.h"
#include "Exception.h"
  
// forward declaration
class CustomTabWidget;

//! Tab Child container
class CustomTabWidget: public QFrame, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
       
  //! child tab flags  
  enum Flags
  {
    //! no flag
    NONE = 0,
        
    //! dock panel stays on top of other windows
    STAYS_ON_TOP = 1
  };
      
  //! constructor
  CustomTabWidget( QTabWidget* parent, const unsigned int& flags = NONE  );
  
  //! dock panel flags
  void setFlags( const unsigned int& flags )
  { flags_ = flags; }
  
  //! set tab title
  void setTitle( const std::string& title )
  { title_ = title; }
  
  //! title
  const std::string& title( void ) const
  { return title_; }
  
  //! get parent TabWidget
  QTabWidget& parentTabWidget( void )
  { 
    Exception::checkPointer( parent_, DESCRIPTION( "parent_ not initialized" ) );
    return *parent_; 
  }
  
  //! get box
  QWidget& box( void )
  { 
    Exception::checkPointer( box_, DESCRIPTION( "box_ not initialized" ) );
    return *box_;
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
  
  protected:
      
  //! close event
  virtual void closeEvent( QCloseEvent* event )
  { 
    if( !parent() ) _toggleDock(); 
    event->ignore();
  }
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  private: 
  
  //! local QSizeGrip
  /*! the paint event method is overridden so that the size grip is invisible */
  class LocalGrip: public QSizeGrip, public Counter
  {
    
    public:
    //! constructor
    LocalGrip( QWidget* parent ):
      QSizeGrip( parent ),
      Counter( "QSizeGrip" )
    {};
    
    protected:
    
    //! paint
    void paintEvent( QPaintEvent* event )
    { QWidget::paintEvent( event ); }
    
  };
  
  //! flags
  unsigned int flags_;
  
  //! title
  std::string title_; 
  
  //! parent CustomTabWidget
  QTabWidget* parent_;
          
  //! vertical layout
  QVBoxLayout* main_layout_;
  
  //! horizontal layout (for buttons)
  QHBoxLayout* button_layout_;
      
  //! contents vbox
  QWidget* box_;
      
  //! size grip
  LocalGrip* size_grip_;
  
  //! button
  QPushButton* button_;
  
  //! index in parent tab
  int index_;
  
  //! default size for the detached panel
  QSize detached_size_;
  
};
#endif

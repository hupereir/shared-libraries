#ifndef _TabWidget_h_
#define _TabWidget_h_

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
  \file TabWidget.h
  \brief Tab widget with detachable pages
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QAction>
#include <QBasicTimer>
#include <QCloseEvent>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QSizeGrip>
#include <QTabWidget>
#include <string>

#include "Counter.h"
#include "Debug.h"
// forward declaration
class TabWidget;

//! Tab Child container
class TabWidget: public QFrame, public Counter
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
  TabWidget( QTabWidget* parent, const unsigned int& flags = NONE  );
  
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
    assert( parent_ );
    return *parent_; 
  }
  
  //! get box
  QWidget& box( void )
  { 
    assert( box_ );
    return *box_;
  }
  
  //! detach action
  QAction& detachAction( void ) const
  { return *detach_action_; }
            
  signals:
  
  //! emmited when box is detached
  void detached( void );
  
  //! emmited when box is attached
  void attached( void );
  
  protected:
      
  //! close event
  virtual void closeEvent( QCloseEvent* event );
  
  //! mouse press event [overloaded]
  virtual void mousePressEvent( QMouseEvent *);
  
  //! mouse move event [overloaded]
  virtual void mouseMoveEvent( QMouseEvent *);     
  
  //! mouse move event [overloaded]
  virtual void mouseReleaseEvent( QMouseEvent *);     
  
  //! mouse move event [overloaded]
  virtual void mouseDoubleClickEvent( QMouseEvent *);     
  
  //! timer event [overloaded]
  virtual void timerEvent( QTimerEvent *);     
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  private: 
  
  //! move enabled
  const bool& _moveEnabled( void ) const
  { return move_enabled_; }
  
  //! move enabled
  void _setMoveEnabled( const bool& value ) 
  { move_enabled_ = value; }
  
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
  
  //! parent TabWidget
  QTabWidget* parent_;
          
  //! vertical layout
  QVBoxLayout* main_layout_;
        
  //! contents vbox
  QWidget* box_;
      
  //! size grip
  LocalGrip* size_grip_;
    
  //! index in parent tab
  int index_;
  
  //! attach/detach action
  QAction* detach_action_;

  //! button state
  Qt::MouseButton button_;
  
  //! move timer
  QBasicTimer timer_;
  
  //! true when move is enabled
  bool move_enabled_;
  
  //! click position
  QPoint click_pos_;
  
};
#endif

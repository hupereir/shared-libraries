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
             
  //! constructor
  TabWidget( QTabWidget* parent );
  
  //! set tab title
  void setTitle( const QString& title )
  { title_ = title; }
  
  //! title
  const QString& title( void ) const
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
  
  //! update actions
  void updateActions( bool );
  
  //! detach action
  QAction& detachAction( void ) const
  { return *detach_action_; }
  
  //! stay on top
  QAction& staysOnTopAction( void ) const
  { return *stays_on_top_action_; }
  
  //! widget is hidden from taskbar
  QAction& stickyAction( void ) const
  { return *sticky_action_; }
  
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
  
  //! actions
  void _installActions( void );
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  //! stays on top
  virtual void _toggleStaysOnTop( bool );
  
  //! toggle window stickyness
  virtual void _toggleSticky( bool );

  private: 
  
  //! move enabled
  const bool& _moveEnabled( void ) const
  { return move_enabled_; }
  
  //! move enabled
  void _setMoveEnabled( const bool& value ) 
  { move_enabled_ = value; }
  
  //! flags
  unsigned int flags_;
  
  //! title
  QString title_; 
  
  //! parent TabWidget
  QTabWidget* parent_;
          
  //! vertical layout
  QVBoxLayout* main_layout_;
        
  //! contents vbox
  QWidget* box_;
      
  //! size grip
  QSizeGrip* size_grip_;
    
  //! index in parent tab
  int index_;
  
  //! attach/detach action
  QAction* detach_action_;

  //! stay on top
  QAction* stays_on_top_action_;
  
  //! make window sticky
  QAction* sticky_action_;
  
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

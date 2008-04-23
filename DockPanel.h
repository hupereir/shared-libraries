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

#include <assert.h>
#include <QAction>
#include <QBasicTimer>
#include <QCloseEvent>
#include <QTimerEvent>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QSizeGrip>

#include <string>

#include "Counter.h"
#include "Debug.h"

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
    assert( panel_ );
    return *panel_;
  }
  
  //! set detachable group panel title
  void setTitle( const std::string& title )
  { title_ = title; }
    
  //! local widget to implement close_event of the content
  class LocalWidget: public QFrame, public Counter
  {
    public:
    
    //! constructor
    LocalWidget( DockPanel* parent );
    
    //! detach action
    QAction& detachAction( void ) const
    { return *detach_action_; }
    
    protected:
    
    //! closeEvent
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
   
    private:
    
    //! move enabled
    const bool& _moveEnabled( void ) const
    { return move_enabled_; }
    
    //! move enabled
    void _setMoveEnabled( const bool& value ) 
    { move_enabled_ = value; }
    
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

  //! main widget
  LocalWidget& main( void ) const
  { return *main_; }

  signals:
  
  //! emmited when state is changed
  void attached( bool state );
  
  //! emmited when panel is attached
  void attached( void );
  
  //! emmited when panel is detached
  void detached( void );
  
  protected slots:
      
  //! toggle dock
  virtual void _toggleDock( void );
  
  protected:
    
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
      
  private:
  
  //! dock title
  std::string title_;
  
  //! flags
  unsigned int flags_;
    
  //! vertical layout for main_ widget
  QVBoxLayout* main_layout_;
  
  //! detachable main widget
  LocalWidget* main_;
    
  //! contents panel
  QWidget* panel_;
    
  //! size grip
  LocalGrip* size_grip_;

  friend class LocalWidget;
  
};

#endif

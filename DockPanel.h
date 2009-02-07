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
        
  //! constructor
  DockPanel( QWidget* parent );

  //! destructor
  virtual ~DockPanel();
     
  //! option name (needed to store sticky and stays-on-top state)
  void setOptionName( QString value )
  {
    sticky_option_name_ = value + "_STICKY";
    stays_on_top_option_name_ = value + "_STAYS_ON_TOP";
    _updateConfiguration();
  }
  
  //! get panel (to add contents)
  virtual QWidget& panel( void )
  { 
    assert( panel_ );
    return *panel_;
  }
  
  //! set detachable group panel title
  void setTitle( const QString& title )
  { title_ = title; }

  //! local widget to implement close_event of the content
  class LocalWidget: public QFrame, public Counter
  {
    
    public:
    
    //! constructor
    LocalWidget( QWidget* parent );
    
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
    
    protected:
    
    //! closeEvent
    virtual void closeEvent( QCloseEvent* event );
    
    //! mouse press event [overloaded]
    virtual void mousePressEvent( QMouseEvent* );
    
    //! mouse move event [overloaded]
    virtual void mouseMoveEvent( QMouseEvent* );     
    
    //! mouse move event [overloaded]
    virtual void mouseReleaseEvent( QMouseEvent* );     
    
    //! mouse move event [overloaded]
    virtual void mouseDoubleClickEvent( QMouseEvent* );     
    
    //! timer event [overloaded]
    virtual void timerEvent( QTimerEvent* );     
    
    //! paint event
    virtual void paintEvent( QPaintEvent* );
    
    //! actions
    void _installActions( void );
    
    private:
    
    //! move enabled
    const bool& _moveEnabled( void ) const
    { return move_enabled_; }
    
    //! move enabled
    void _setMoveEnabled( const bool& value ) 
    { move_enabled_ = value; }
    
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
  
  //! visibility changed
  void visibilityChanged( bool );
  
  
  protected slots:
  
  //! toggle dock
  virtual void _toggleDock( void );
  
  //! stays on top
  virtual void _toggleStaysOnTop( bool );
  
  //! toggle window stickyness
  virtual void _toggleSticky( bool );

  protected:

  //! hide event
  virtual void hideEvent( QHideEvent* );
    
  private slots:
  
  //! configuration
  void _updateConfiguration( void );
  
  private:
  
  //! true if option name is set
  bool _hasOptionName( void ) const
  { return !(_stickyOptionName().isEmpty() || _staysOnTopOptionName().isEmpty() ); }
  
  //! option name
  const QString& _stickyOptionName( void ) const
  { return sticky_option_name_; }
  
  //! option name
  const QString& _staysOnTopOptionName( void ) const
  { return stays_on_top_option_name_; }

  //! dock title
  QString title_;
  
  //! option name 
  /*! needed to store sticky and stays on top state */
  QString sticky_option_name_;
  
  //! option name
  QString stays_on_top_option_name_;
  
  //! vertical layout for main_ widget
  QVBoxLayout* main_layout_;
  
  //! detachable main widget
  LocalWidget* main_;
    
  //! contents panel
  QWidget* panel_;
    
  //! size grip
  QSizeGrip* size_grip_;

  friend class LocalWidget;
  
};

#endif

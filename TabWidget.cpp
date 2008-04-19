
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
  \file TabWidget.cpp
  \brief Tab widget with detachable pages
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QGridLayout>

#include "TabWidget.h"
#include "File.h"
#include "XmlOptions.h"
#include "X11Util.h"

using namespace std;

//________________________________________________________
TabWidget::TabWidget( QTabWidget* parent, const unsigned int& flags ):
    QFrame(), 
    Counter( "TabWidget" ),
    flags_( flags ),
    parent_( parent ),
    index_( 0 ),
    button_( Qt::NoButton ),
    move_enabled_( false )
{

  Debug::Throw( "TabWidget::TabWidget.\n" );
  setFrameStyle( QFrame::NoFrame );

  // grid layout to overlay main layout and invisible grip
  QGridLayout *grid_layout( new QGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(0);
  setLayout( grid_layout );

  grid_layout->addLayout( main_layout_ = new QVBoxLayout(), 0, 0, 1, 1 );
  main_layout_->setMargin(5);
  main_layout_->setSpacing(2);
  
  // vertical box
  box_ = new QWidget( this );
  box().setLayout( new QVBoxLayout() );
  box().layout()->setSpacing( 2 );
  box().layout()->setMargin( 0 );
  
  main_layout_->addWidget( box_ );
   
  // size grip
  grid_layout->addWidget( size_grip_ = new LocalGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
  size_grip_->hide(); 
 
  // detach action
  detach_action_ = new QAction( "&detach", this );
  detachAction().setToolTip( "dock/undock panel" );
  connect( &detachAction(), SIGNAL( triggered() ), SLOT( _toggleDock() ) );
  addAction( &detachAction() );
  setContextMenuPolicy( Qt::ActionsContextMenu );
  
}

//___________________________________________________________
void TabWidget::_toggleDock( void )
{
  
  Debug::Throw( "TabWidget::_toggleDock.\n" );

  if( !parent() ) {
    
    // store size for later detach
    detached_size_ = size();
    detachAction().setText("&detach");
    
    // reinsert into parent and select
    setFrameStyle( QFrame::NoFrame );
    parent_->QTabWidget::insertTab( index_, this, title_.c_str() );
    parent_->QTabWidget::setCurrentWidget( this );
    
    // modify button text
    size_grip_->hide();
        
    emit attached();
    
  } else {
    
    // keep track of index, for later re-insertion
    // and remove from parent TabWidget
    index_ = parent_->indexOf( this );
    parent_->removeTab( index_ );
       
    // keep track of parent
    QWidget *parent( parentWidget() );
    
    // reparent to top level
    setParent( 0 );
 
    // window flags
    Qt::WindowFlags flags = Qt::FramelessWindowHint;
    if( flags_ & STAYS_ON_TOP ) flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( flags );

    // frame style
    setFrameStyle( QFrame::Panel | QFrame::Raised );
   
    // move and resize
    move( parent->mapToGlobal( QPoint(0,0) ) );    
    setWindowIcon( QPixmap(File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
    if( !title_.empty() ) { setWindowTitle( title_.c_str() ); }
    if( detached_size_ != QSize() ) resize( detached_size_ );

    // change action text
    detachAction().setText("&attach");

    // show widgets
    size_grip_->show();
    show();
    
    // signal
    emit detached();
  }  
  
}

//___________________________________________________________
void TabWidget::closeEvent( QCloseEvent* event )
{
  Debug::Throw( "TabWidget::closeEvent.\n" );
  if( !parent() ) detachAction().trigger();
  event->ignore();
}

//___________________________________________________________
void TabWidget::mousePressEvent( QMouseEvent* event )
{
  Debug::Throw( "TabWidget::mousePressEvent.\n" );
  button_ = event->button();
  if( button_ == Qt::LeftButton ) 
  { 
    click_pos_ = event->pos() + QPoint(geometry().topLeft() - frameGeometry().topLeft()); 
    timer_.start( 200, this );
  }
  
  return QFrame::mousePressEvent( event );
}

//___________________________________________________________
void TabWidget::mouseReleaseEvent( QMouseEvent* event )
{
  Debug::Throw( "TabWidget::mouseReleaseEvent.\n" );
  button_ = Qt::NoButton;
  _setMoveEnabled( false );
  unsetCursor();
  timer_.stop();
  return QFrame::mouseReleaseEvent( event );
}

//___________________________________________________________
void TabWidget::mouseMoveEvent( QMouseEvent* event )
{
 
  // check button
  if( button_ != Qt::LeftButton ) return QFrame::mouseMoveEvent( event );

  // if not yet enabled, enable immediately and stop timer
  if( !_moveEnabled() ) {
    
    timer_.stop();
    if( parent() ) detachAction().trigger();    
    if( X11Util::moveWidget( *this, QCursor::pos() ) ) return; 
    else { 
      
      // enable
      _setMoveEnabled( true );
      setCursor( Qt::SizeAllCursor );
      
    }
          
  }

  // move widget, the standard way
  QPoint point(event->globalPos() - click_pos_ );
  move( point );

}

//___________________________________________________________
void TabWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
  Debug::Throw( "TabWidget::mouseDoubleClickEvent.\n" );
  detachAction().trigger();
  timer_.stop();
}

//___________________________________________________________
void TabWidget::timerEvent( QTimerEvent *event )
{
  
  Debug::Throw( "TabWidget::timerEvent.\n" );
  if( event->timerId() != timer_.timerId() ) return QFrame::timerEvent( event );
  
  timer_.stop();
  if( button_ != Qt::LeftButton ) return;
  
  if( parent() ) detachAction().trigger();
  if( X11Util::moveWidget( *this, QCursor::pos() ) ) return; 
  else {
    _setMoveEnabled( true );
    setCursor( Qt::SizeAllCursor );
  }
  
}

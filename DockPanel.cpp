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
  \file DockPanel.cpp
  \brief detachable generic panel
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QGridLayout>
#include <QApplication>

#include "DockPanel.h"
#include "Debug.h"
#include "File.h"
#include "XmlOptions.h"
#include "X11Util.h"

using namespace std;

//___________________________________________________________
DockPanel::DockPanel( QWidget* parent, const unsigned int& flags ):
  QWidget( parent ),
  Counter( "DockPanel" ),
  flags_( flags )
{
  Debug::Throw( "DockPanel::DockPanel.\n" );
  
  // this layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(2);
  
  // main widget
  layout()->addWidget( main_ = new LocalWidget( this ) );
  _main().setFrameStyle( QFrame::StyledPanel|QFrame::Plain );
  connect( &_main().detachAction(), SIGNAL( triggered() ), SLOT( _toggleDock() ) );
  
  Debug::Throw( "DocPanel::DockPanel - main_layout.\n" );

  // grid layout to overlay main layout and invisible grip
  QGridLayout *grid_layout( new QGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(0);
  _main().setLayout( grid_layout );
  
  // vertical layout for children
  main_layout_ = new QVBoxLayout();
  main_layout_->setMargin( 5 );
  main_layout_->setSpacing( 5 );
  grid_layout->addLayout( main_layout_, 0, 0, 1, 1 );
  
  // vertical panel
  Debug::Throw( "DocPanel::DockPanel - panel.\n" );
  panel_ = new QWidget( main_ );
  panel_->setLayout( new QVBoxLayout() );
  panel_->layout()->setMargin(0);
  panel_->layout()->setSpacing(2);
  
  main_layout_->addWidget( panel_, 1 );
 
  // invisible size grip
  grid_layout->addWidget( size_grip_ = new LocalGrip( main_ ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
  size_grip_->hide(); 
    
}

//___________________________________________________________
void DockPanel::_toggleDock( void )
{
  
  Debug::Throw( "DockPanel::_toggleDock.\n" );

  if( !_main().parent() ) 
  {
  
    detached_size_ = _main().size();
    _main().setParent( this );
    layout()->addWidget( main_ );
    _main().setFrameStyle( QFrame::StyledPanel|QFrame::Plain );
    size_grip_->hide();
    _main().show();
    
    // change action text
    _main().detachAction().setText("&detach");
    
    // signals
    emit attached( true );
    emit attached();
  
  } else {
        
    _main().setParent( 0 );
    
    // window flags
    Qt::WindowFlags flags = Qt::FramelessWindowHint;
    if( flags_ & STAYS_ON_TOP ) flags |= Qt::WindowStaysOnTopHint;
    _main().setWindowFlags( flags );

    // frame style
    _main().setFrameStyle( QFrame::Panel | QFrame::Raised );

    // move and resize
    _main().move( mapToGlobal( QPoint(0,0) ) );
    _main().setWindowIcon( QPixmap(File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
    if( !title_.empty() ) _main().setWindowTitle( title_.c_str() );
    if( detached_size_ != QSize() ) _main().resize( detached_size_ );    
    
    // change action text
    _main().detachAction().setText("&attach");
    
    // show widgets
    size_grip_->show();
    _main().show();
    
    // signals
    emit attached( false );
    emit detached();
  
  }  
  
}

//___________________________________________________________
DockPanel::LocalWidget::LocalWidget( DockPanel* parent ):
  QFrame( parent ),
  Counter( "DockPanel::LocalWidget" ),
  button_( Qt::NoButton ),
  move_enabled_( false )
{
  detach_action_ = new QAction( "&detach", this );
  detach_action_->setToolTip( "dock/undock panel" );
  addAction( detach_action_ );
  setContextMenuPolicy( Qt::ActionsContextMenu );
}

//___________________________________________________________
void DockPanel::LocalWidget::closeEvent( QCloseEvent* event )
{
  Debug::Throw( "DockPanel::LocalWidget::closeEvent.\n" );
  if( !parent() ) detachAction().trigger();
  event->ignore();
}

//___________________________________________________________
void DockPanel::LocalWidget::mousePressEvent( QMouseEvent* event )
{
  Debug::Throw( "DockPanel::LocalWidget::mousePressEvent.\n" );
  button_ = event->button();
      
  if( button_ == Qt::LeftButton ) 
  { 
    click_pos_ = event->pos() + QPoint(geometry().topLeft() - frameGeometry().topLeft()); 
    timer_.start( 200, this );
  }
  
  return QFrame::mousePressEvent( event );
}

//___________________________________________________________
void DockPanel::LocalWidget::mouseReleaseEvent( QMouseEvent* event )
{
  Debug::Throw( "DockPanel::LocalWidget::mouseReleaseEvent.\n" );
  button_ = Qt::NoButton;
  _setMoveEnabled( false );
  unsetCursor();
  timer_.stop();
  return QFrame::mouseReleaseEvent( event );
}

//___________________________________________________________
void DockPanel::LocalWidget::mouseMoveEvent( QMouseEvent* event )
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
void DockPanel::LocalWidget::mouseDoubleClickEvent( QMouseEvent* event )
{
  Debug::Throw( "DockPanel::LocalWidget::mouseDoubleClickEvent.\n" );
  detachAction().trigger();
  timer_.stop();
}

//___________________________________________________________
void DockPanel::LocalWidget::timerEvent( QTimerEvent *event )
{
  
  Debug::Throw( "DockPanel::LocalWidget::timerEvent.\n" );
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


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
TabWidget::TabWidget( QTabWidget* parent ):
    QFrame(),
    Counter( "TabWidget" ),
    parent_( parent ),
    size_grip_( 0 ),
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
  //if( XmlOptions::get().get<bool>( "SIZE_GRIP_ENABLED" ) )
  {
    grid_layout->addWidget( size_grip_ = new QSizeGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
    _hideSizeGrip();
  }

  _installActions();
  updateActions( false );
  setContextMenuPolicy( Qt::ActionsContextMenu );

}


//___________________________________________________________
void TabWidget::updateActions( bool detached )
{

  detachAction().setText( detached ? "&Attach":"&Detach" );
  stickyAction().setEnabled( detached );
  staysOnTopAction().setEnabled( detached );

}

//___________________________________________________________
void TabWidget::_toggleDock( void )
{

  Debug::Throw( "TabWidget::_toggleDock.\n" );

  if( !parent() ) {

    // store size for later detach
    updateActions( false );

    // reinsert into parent and select
    setFrameStyle( QFrame::NoFrame );
    parent_->QTabWidget::insertTab( index_, this, title_ );
    parent_->QTabWidget::setCurrentWidget( this );

    // modify button text
    _hideSizeGrip();

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
    //setFrameStyle( QFrame::Panel | QFrame::Raised );
    setFrameStyle( QFrame::NoFrame );

    // window flags
    Qt::WindowFlags flags = Qt::FramelessWindowHint|Qt::Tool;
    setWindowFlags( flags );

    // move and resize
    move( parent->mapToGlobal( QPoint(0,0) ) );

    // warning: innefficient
    setWindowIcon( QPixmap( XmlOptions::get().raw( "ICON_PIXMAP" ) ) );

    if( !title_.isEmpty() ) { setWindowTitle( title_ ); }

    // change action text
    updateActions( true );

    _toggleStaysOnTop( staysOnTopAction().isChecked() );
    _toggleSticky( stickyAction().isChecked() );

    // show widgets
    _showSizeGrip();
    show();

    // signal
    emit detached();
  }

}


//__________________________________________________________
void TabWidget::_toggleStaysOnTop( bool state )
{

  // check that widget is top level
  if( parentWidget() ) return;

  bool visible( !isHidden() );
  if( visible ) hide();

  #ifdef Q_WS_X11

  // on X11 one uses NET_WM directly
  // because the QT equivalent conflicts with the STICKY flag below

  // change property depending on state
  if( state )
  {

    X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_STAYS_ON_TOP, 1 );
    X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_ABOVE, 1 );

  } else {

    X11Util::get().removeProperty( *this, X11Util::_NET_WM_STATE_STAYS_ON_TOP);
    X11Util::get().removeProperty( *this, X11Util::_NET_WM_STATE_ABOVE);

  }

  #else

  // Qt implementation
  if( state ) setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint );
  else setWindowFlags( windowFlags() & ~Qt::WindowStaysOnTopHint );

  #endif
  if( visible ) show();

}

//__________________________________________________________
void TabWidget::_toggleSticky( bool state )
{

  Debug::Throw( "TabWidget::_toggleSticky.\n" );

  // check that widget is top level
  if( parentWidget() ) return;

  #ifdef Q_WS_X11
  if( X11Util::get().isSupported( X11Util::_NET_WM_STATE_STICKY ) )
  {

    // the widget must first be hidden
    // prior to modifying the property
    bool visible( !isHidden() );
    if( visible ) hide();

    // change property depending on state
    if( state ) X11Util::get().changeProperty( *this, X11Util::_NET_WM_STATE_STICKY, 1);
    else X11Util::get().removeProperty( *this, X11Util::_NET_WM_STATE_STICKY );

    // show widget again, if needed
    if( visible ) show();

  } else if( X11Util::get().isSupported( X11Util::_NET_WM_DESKTOP ) ) {

    // the widget must first be hidden
    // prior to modifying the property
    bool visible( !isHidden() );
    if( visible ) hide();

    X11Util::get().changeCardinal( *this, X11Util::_NET_WM_DESKTOP, state ? X11Util::ALL_DESKTOPS:0 );

    // show widget again, if needed
    if( visible ) show();

  }

  #endif

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
    if( X11Util::get().moveWidget( *this, QCursor::pos() ) ) return;
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
  if( X11Util::get().moveWidget( *this, QCursor::pos() ) ) return;
  else {
    _setMoveEnabled( true );
    setCursor( Qt::SizeAllCursor );
  }

}

//____________________________________________________
void TabWidget::_installActions( void )
{

  Debug::Throw( "TabWidget::_installActions.\n" );

  // detach action
  addAction( detach_action_ = new QAction( "&Detach", this ) );
  detach_action_->setToolTip( "Dock/undock panel" );
  connect( detach_action_, SIGNAL( triggered() ), SLOT( _toggleDock() ) );

  // stays on top
  addAction( stays_on_top_action_ = new QAction( "&Stays on Top", this ) );
  stays_on_top_action_->setToolTip( "Keep window on top of all others" );
  stays_on_top_action_->setCheckable( true );
  stays_on_top_action_->setChecked( false );
  connect( stays_on_top_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleStaysOnTop( bool ) ) );

  // sticky
  addAction( sticky_action_ = new QAction( "&Sticky", this ) );
  sticky_action_->setToolTip( "Make window appear on all desktops" );
  sticky_action_->setCheckable( true );
  sticky_action_->setChecked( false );
  connect( sticky_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleSticky( bool ) ) );

}

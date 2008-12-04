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

#include "DockPanel.h"
#include "Debug.h"
#include "File.h"
#include "Singleton.h"
#include "XmlOptions.h"
#include "X11Util.h"

using namespace std;

//___________________________________________________________
DockPanel::DockPanel( QWidget* parent ):
  QWidget( parent ),
  Counter( "DockPanel" )
{
  Debug::Throw( "DockPanel::DockPanel.\n" );

  // this layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(2);

  // main widget
  layout()->addWidget( main_ = new LocalWidget( this ) );
  main().setFrameStyle( QFrame::StyledPanel|QFrame::Raised );
  connect( &main().detachAction(), SIGNAL( triggered() ), SLOT( _toggleDock() ) );

  // connections
  main().updateActions( false );
  connect( &main().stickyAction(), SIGNAL( toggled( bool ) ), SLOT( _toggleSticky( bool ) ) );
  connect( &main().staysOnTopAction(), SIGNAL( toggled( bool ) ), SLOT( _toggleStaysOnTop( bool ) ) );

  Debug::Throw( "DocPanel::DockPanel - main_layout.\n" );

  // grid layout to overlay main layout and invisible grip
  QGridLayout *grid_layout( new QGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(0);
  main().setLayout( grid_layout );

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

  // connections
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  
}

//___________________________________________________________
void DockPanel::_toggleDock( void )
{

  Debug::Throw( "DockPanel::_toggleDock.\n" );

  if( !main().parent() )
  {

    // change parent
    main().setParent( this );
    layout()->addWidget( main_ );
    size_grip_->hide();
    main().setFrameStyle( QFrame::StyledPanel|QFrame::Raised );
    main().show();

    // change action text
    main().updateActions( false );

    // signals
    emit attached( true );
    emit attached();

  } else {

    // change parent
    main().setParent( 0 );
    main().setFrameStyle( QFrame::Panel|QFrame::Raised );

    // window flags
    Qt::WindowFlags flags = Qt::FramelessWindowHint|Qt::Tool;
    main().setWindowFlags( flags );

    // move and resize
    main().move( mapToGlobal( QPoint(0,0) ) );
    main().setWindowIcon( QPixmap(File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
    if( !title_.empty() ) main().setWindowTitle( title_.c_str() );

    // change action text
    main().updateActions( true );

    _toggleStaysOnTop( main().staysOnTopAction().isChecked() );
    _toggleSticky( main().stickyAction().isChecked() );

    // show widgets
    size_grip_->show();
    main().show();

    // signals
    emit attached( false );
    emit detached();

  }

}

//__________________________________________________________
void DockPanel::_toggleStaysOnTop( bool state )
{

  Debug::Throw( "DockPanel::_toggleStaysOnTop.\n" );

  // check that widget is top level
  if( main().parentWidget() ) return;

  bool visible( !main().isHidden() );
  if( visible ) main().hide();

  #ifdef Q_WS_X11

  // on X11 one uses NET_WM directly
  // because the QT equivalent conflicts with the STICKY flag below

  // change property depending on state
  if( state )
  {

    X11Util::changeProperty( main(), X11Util::_NET_WM_STATE_STAYS_ON_TOP, 1 );
    X11Util::changeProperty( main(), X11Util::_NET_WM_STATE_ABOVE, 1 );

  } else {

    X11Util::removeProperty( main(), X11Util::_NET_WM_STATE_STAYS_ON_TOP);
    X11Util::removeProperty( main(), X11Util::_NET_WM_STATE_ABOVE);

  }

  #else

  // Qt implementation
  if( state ) main().setWindowFlags( main().windowFlags() | Qt::WindowStaysOnTopHint );
  else main().setWindowFlags( main().windowFlags() & ~Qt::WindowStaysOnTopHint );

  #endif
  if( visible ) main().show();

  // update option if any
  if( _hasOptionName() )
  { 
    XmlOptions::get().set<bool>( _staysOnTopOptionName(), state ); 
    Debug::Throw() << "DockPanel::_toggleStaysOnTop - " << _staysOnTopOptionName() << ": " << state << endl;
  }

}

//__________________________________________________________
void DockPanel::_toggleSticky( bool state )
{

  Debug::Throw( "DockPanel::_toggleSticky.\n" );

  // check that widget is top level
  if( main().parentWidget() ) return;

  // the widget must first be hidden
  // prior to modifying the property
  bool visible( !main().isHidden() );
  if( visible ) main().hide();

  // change property depending on state
  if( state ) X11Util::changeProperty( main(), X11Util::_NET_WM_STATE_STICKY, 1 );
  else X11Util::removeProperty( main(), X11Util::_NET_WM_STATE_STICKY );

  // show widget again, if needed
  if( visible ) main().show();

  // update option if any
  if( _hasOptionName() )
  { 
    XmlOptions::get().set<bool>( _stickyOptionName(), state ); 
    Debug::Throw() << "DockPanel::_toggleSticky - " << _stickyOptionName() << ": " << state << endl;
  }
  
}

//___________________________________________________________
void DockPanel::_updateConfiguration( void )
{
  
  Debug::Throw( "DockPanel::_updateConfiguration.\n" );
  
  if( _hasOptionName() )
  {
    if( XmlOptions::get().find( _staysOnTopOptionName() ) ) main().staysOnTopAction().setChecked( XmlOptions::get().get<bool>( _staysOnTopOptionName() ) ); 
    if( XmlOptions::get().find( _stickyOptionName() ) ) main().stickyAction().setChecked( XmlOptions::get().get<bool>( _stickyOptionName() ) ); 
  }
  
}

//___________________________________________________________
DockPanel::LocalWidget::LocalWidget( QWidget* parent ):
  QFrame( parent ),
  Counter( "LocalWidget" ),
  button_( Qt::NoButton ),
  move_enabled_( false )
{
  _installActions();
  setContextMenuPolicy( Qt::ActionsContextMenu );
}

//___________________________________________________________
void DockPanel::LocalWidget::updateActions( bool detached )
{

  detachAction().setText( detached ? "&attach":"&detach" );
  stickyAction().setEnabled( detached );
  staysOnTopAction().setEnabled( detached );

}

//___________________________________________________________
void DockPanel::hideEvent( QHideEvent* event )
{
  Debug::Throw( "DockPanel::hideEvent.\n" );
  emit visibilityChanged( false );
  QWidget::hideEvent( event );
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
  if( !_moveEnabled() ) return;

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

//___________________________________________________________
void DockPanel::LocalWidget::_installActions( void )
{

  Debug::Throw( "DockPanel::LocalWidget::_installActions.\n" );

  // detach
  addAction( detach_action_ = new QAction( "&detach", this ) );
  detach_action_->setToolTip( "dock/undock panel" );

  // stays on top
  addAction( stays_on_top_action_ = new QAction( "&Stays on top", this ) );
  stays_on_top_action_->setToolTip( "keep window on top of all others" );
  stays_on_top_action_->setCheckable( true );

  // sticky
  addAction( sticky_action_ = new QAction( "&Sticky", this ) );
  sticky_action_->setToolTip( "make window appear on all desktops" );
  sticky_action_->setCheckable( true );

}

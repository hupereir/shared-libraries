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

#include "DockPanel.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

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

  // vertical layout for children
  mainLayout_ = new QVBoxLayout();
  mainLayout_->setMargin( 5 );
  mainLayout_->setSpacing( 5 );
  main().setLayout( mainLayout_ );

  // vertical panel
  Debug::Throw( "DocPanel::DockPanel - panel.\n" );
  panel_ = new QWidget( main_ );
  panel_->setLayout( new QVBoxLayout() );
  panel_->layout()->setMargin(0);
  panel_->layout()->setSpacing(2);

  mainLayout_->addWidget( panel_, 1 );

  // connections
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

}

//___________________________________________________________
DockPanel::~DockPanel( void )
{

  Debug::Throw( "DockPanel::~DockPanel.\n" );

  // in detached mode, the panel must be explicitely deleted,
  // because it does not have the right parent
  if( !main().parent() ) { main().deleteLater(); }

}

//___________________________________________________________
QSize DockPanel::minimumSizeHint( void ) const
{ return QSize(0,0); }

//___________________________________________________________
void DockPanel::_toggleDock( void )
{

  Debug::Throw( "DockPanel::_toggleDock.\n" );

  if( !main().parent() )
  {

    // change parent
    main().setParent( this );
    layout()->addWidget( main_ );

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
    main().setFrameStyle( QFrame::NoFrame );

    // window flags
    main().setWindowFlags( Qt::FramelessWindowHint|Qt::Window );

    // move and resize
    main().move( mapToGlobal( QPoint(0,0) ) );

    // warning: innefficient
    main().setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand() ) );
    if( !title_.isEmpty() ) main().setWindowTitle( title_ );

    // change action text
    main().updateActions( true );

    _toggleStaysOnTop( main().staysOnTopAction().isChecked() );
    _toggleSticky( main().stickyAction().isChecked() );

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

  #ifdef Q_WS_X11

  X11Util::get().changeProperty( main(), X11Util::_NET_WM_STATE_STAYS_ON_TOP, state );
  X11Util::get().changeProperty( main(), X11Util::_NET_WM_STATE_ABOVE, state );

  #else

  bool visible( !main().isHidden() );
  if( visible ) main().hide();

  // Qt implementation
  if( state ) main().setWindowFlags( main().windowFlags() | Qt::WindowStaysOnTopHint );
  else main().setWindowFlags( main().windowFlags() & ~Qt::WindowStaysOnTopHint );

  if( visible ) main().show();

  #endif

  // update option if any
  if( _hasOptionName() ) XmlOptions::get().set<bool>( _staysOnTopOptionName(), state );

}

//__________________________________________________________
void DockPanel::_toggleSticky( bool state )
{

  Debug::Throw( "DockPanel::_toggleSticky.\n" );

  // check that widget is top level
  if( main().parentWidget() ) return;

  #ifdef Q_WS_X11
  if( X11Util::get().isSupported( X11Util::_NET_WM_STATE_STICKY ) )
  {

    X11Util::get().changeProperty( main(), X11Util::_NET_WM_STATE_STICKY, state );

  } else if( X11Util::get().isSupported( X11Util::_NET_WM_DESKTOP ) ) {

    unsigned long desktop = X11Util::get().cardinal( QX11Info::appRootWindow(), X11Util::_NET_CURRENT_DESKTOP );
    X11Util::get().changeCardinal( main(), X11Util::_NET_WM_DESKTOP, state ? X11Util::ALL_DESKTOPS:desktop );

  }

  #endif

  return;

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
  moveEnabled_( false )
{
  _installActions();
  setContextMenuPolicy( Qt::ActionsContextMenu );
  setAttribute(Qt::WA_TranslucentBackground);
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
    clickPos_ = event->pos() + QPoint(geometry().topLeft() - frameGeometry().topLeft());
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
  QPoint point(event->globalPos() - clickPos_ );
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
  if( X11Util::get().moveWidget( *this, QCursor::pos() ) ) return;
  else {
    _setMoveEnabled( true );
    setCursor( Qt::SizeAllCursor );
  }

}

//___________________________________________________________
void DockPanel::LocalWidget::paintEvent( QPaintEvent *event )
{
  if( parentWidget() || !QX11Info::isCompositingManagerRunning() ) { QFrame::paintEvent( event ); }
  else {

    if( tileSet_.isEmpty() )
    {
      const int size = 5;
      QPixmap pixmap = QPixmap( size*2, size*2 );
      pixmap.fill( Qt::transparent );

      QPainter p( &pixmap );
      p.setRenderHint( QPainter::Antialiasing );
      p.setPen( Qt::NoPen );

      QColor color( palette().window().color() );
      p.setBrush( color );

      // draw ellipse.
      p.drawEllipse( QRectF( size-4, size-4, 8, 8 ) );
      tileSet_ = TileSet(pixmap, size, size, 1, 1);

    }

    QPainter p( this );
    tileSet_.render( rect(), &p );
    QColor color( palette().window().color() );
    p.setBrush( color );
    p.setPen( Qt::NoPen );
    p.drawRect( rect().adjusted(4, 4, -4, -4 ) );

  }

}

//___________________________________________________________
void DockPanel::LocalWidget::_installActions( void )
{

  Debug::Throw( "DockPanel::LocalWidget::_installActions.\n" );

  // detach
  addAction( detachAction_ = new QAction( "&Detach", this ) );
  detachAction_->setToolTip( "Dock/undock panel" );

  // stays on top
  addAction( staysOnTopAction_ = new QAction( "&Stays on Top", this ) );
  staysOnTopAction_->setToolTip( "Keep window on top of all others" );
  staysOnTopAction_->setCheckable( true );

  // sticky
  addAction( stickyAction_ = new QAction( "&Sticky", this ) );
  stickyAction_->setToolTip( "Make window appear on all desktops" );
  stickyAction_->setCheckable( true );

}

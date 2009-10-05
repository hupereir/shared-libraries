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
  \file TransitionWidget.h
  \brief widget used for smooth transition between two widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>
#include <QApplication>

#include "Debug.h"
#include "Singleton.h"
#include "TransitionWidget.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________________________________
TransitionWidget::TransitionWidget( QWidget *parent ):
  QWidget( parent ),
  Counter( "TransitionWidget" ),
  enable_on_options_( true ),
  enabled_( true ),
  flags_( FROM_PARENT | SHOW )
{
  Debug::Throw( "TransitionWidget::TransitionWidget.\n" );
  connect( &timeLine(), SIGNAL( frameChanged( int ) ), this, SLOT( update( void )) );
  connect( Singleton::get().application(), SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) );
  _updateConfiguration();
}

//___________________________________________________________________
TransitionWidget::~TransitionWidget()
{ Debug::Throw( "TransitionWidget::~TransitionWidget.\n" ); }

//___________________________________________________________________
void TransitionWidget::initialize( QWidget* widget, QRect rect )
{

  // check widget
  if( !widget ) widget = parentWidget();
  assert( widget );

  // change rect
  if( rect.isNull() ) rect = widget->rect();
  setGeometry( rect );

  // use parent window instead of widget if requested
  if( flags() & FROM_PARENT )
  {
    rect = rect.translated( widget->mapTo( widget->window(), widget->rect().topLeft() ) );
    widget = widget->window();
  }

  // grab pixmap
  pixmap_ = QPixmap::grabWidget( widget, rect );

  // show widget
  if( flags() & SHOW ) TransitionWidget::show();

}

//___________________________________________________________________
void TransitionWidget::start( void )
{ if( timeLine().state() != QTimeLine::Running ) timeLine().start(); }

//___________________________________________________________________
void TransitionWidget::mousePressEvent( QMouseEvent* )
{ if( isVisible() ) hide(); }

//___________________________________________________________________
void TransitionWidget::paintEvent( QPaintEvent* event )
{

  // create painter
  QPainter painter( this );
  //painter.setClipRect( event->rect() );
  painter.fillRect( rect(), Qt::transparent );
  painter.setRenderHints(QPainter::SmoothPixmapTransform);

  #ifdef Q_WS_WIN
  /*
  under windows, it seems composition mode must be set to source
  explicitely, at least for applications that use true translucency
  otherwise, the transparency effects are messed-up. Other applications
  are apparently not affected. On linux on the other hand, non-compositing
  applications show wrong effects if this is on
  */
  painter.setCompositionMode( QPainter::CompositionMode_Source );
  #endif

  if( timeLine().state() == QTimeLine::Running )
  {

    qreal opacity( 1.0 - qreal( timeLine().currentFrame() )/qreal( timeLine().endFrame() ) );
    painter.setOpacity( opacity );

  }


  // draw pixmap
  painter.drawPixmap( QPoint(0,0), pixmap_ );
  painter.end();

}

//___________________________________________________________________
void TransitionWidget::_updateConfiguration( void )
{
  Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );

  if( enable_on_options_ ) setEnabled(  XmlOptions::get().get<bool>( "SMOOTH_TRANSITION_ENABLED" ) );
  timeLine().setDuration( XmlOptions::get().get<int>( "SMOOTH_TRANSITION_DURATION" ) );
  timeLine().setFrameRange( 0, XmlOptions::get().get<int>( "ANIMATION_FRAMES" ) );

}

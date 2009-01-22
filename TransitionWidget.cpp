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
  enabled_( true )
{
  Debug::Throw( "TransitionWidget::TransitionWidget.\n" );
  connect( &timeLine(), SIGNAL(frameChanged(int)), this, SLOT( _update()) ); 
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//___________________________________________________________________
void TransitionWidget::setStartWidget( QWidget* widget, QRect rect, bool from_parent )
{
  
  //Debug::Throw( "TransitionWidget::setStartWidget.\n" );
  assert( widget );
  if( rect.isNull() ) rect = widget->rect();
  if( from_parent )
  {
    rect = rect.translated( widget->mapTo( widget->window(), widget->rect().topLeft() ) );
    widget = widget->window(); 
  }
  first_ = QPixmap::grabWidget( widget, rect );
  Debug::Throw( "TransitionWidget::setStartWidget - done.\n" );
  
}

//___________________________________________________________________
void TransitionWidget::start( void )
{ 
  //Debug::Throw( "TransitionWidget::start.\n" );
  if( timeLine().state() != QTimeLine::Running ) timeLine().start(); 
}

//___________________________________________________________________
void TransitionWidget::paintEvent( QPaintEvent* event )
{
  
  Debug::Throw( "TransitionWidget::paintEvent.\n" );
  
  qreal frame = timeLine().currentFrame();
  
  bool running( timeLine().state() == QTimeLine::Running );

  QPainter painter( this );
  painter.setClipRect( event->rect() );
  painter.fillRect( rect(), Qt::transparent );
  
  painter.setRenderHints(QPainter::SmoothPixmapTransform);
  if( running ) {
    
    painter.setOpacity( double(frame)/timeLine().startFrame() );
    painter.drawPixmap( QPoint(0,0), first_ );
    painter.end();
    
  } else painter.drawPixmap( QPoint(0,0), first_ );
    
  if( running && frame <= 0 ) { timeLine().stop(); }
  
}

//___________________________________________________________________
void TransitionWidget::_update( void )
{
  Debug::Throw( "TransitionWidget::_update.\n" );
  update();
}
  
//___________________________________________________________________
void TransitionWidget::_updateConfiguration( void )
{
  Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );
  setEnabled(  XmlOptions::get().get<bool>( "ENABLE_ANIMATIONS" ) );
  timeLine().setDuration( XmlOptions::get().get<int>( "ANIMATION_DURATION" ) );
  timeLine().setFrameRange( XmlOptions::get().get<int>( "ANIMATION_FRAMES" ), 0 );
} 

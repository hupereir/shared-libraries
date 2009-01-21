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
  enabled_( true ),
  fading_mode_( FADE_BOTH ),
  copy_mode_( RENDER )
{
  Debug::Throw( "TransitionWidget::TransitionWidget.\n" );
  connect( &timeLine(), SIGNAL(frameChanged(int)), this, SLOT(update())); 
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//___________________________________________________________________
void TransitionWidget::setStartWidget( QWidget* widget )
{ 
  Debug::Throw( "TransitionWidget::setStartWidget.\n" );
  switch( copy_mode_ )
  {
    case RENDER:
    first_ = Pixmap( size() );
    first_.fromWidget( widget ); 
    break;
    
    case GRAB:
    first_ = QPixmap::grabWidget( widget, widget->rect() );
    break;
    
    default: 
    assert(0);
  }
  
}


//___________________________________________________________________
void TransitionWidget::setEndWidget( QWidget* widget )
{ 
  Debug::Throw( "TransitionWidget::setEndWidget.\n" );
  switch( copy_mode_ )
  {
    case RENDER:
    second_ = Pixmap( size() );
    second_.fromWidget( widget ); 
    break;
    
    case GRAB:
    first_ = QPixmap::grabWidget( widget, widget->rect() );
    break;
    
    default: 
    assert(0);
  }
    
}

//___________________________________________________________________
void TransitionWidget::start( QSize size, QWidget* first, QWidget* second )
{
  Debug::Throw( "TransitionWidget::start.\n" );
  resize( size );
  setStartWidget( first );
  setEndWidget( second );
  start();
}

//___________________________________________________________________
void TransitionWidget::start( void )
{ if( timeLine().state() != QTimeLine::Running ) timeLine().start(); }

//___________________________________________________________________
void TransitionWidget::paintEvent( QPaintEvent* event )
{
  
  qreal frame = timeLine().currentFrame();
  
  bool running( timeLine().state() == QTimeLine::Running );
  if( fading_mode_ != NONE )
  {
    QPainter painter( this );
    painter.fillRect( rect(), Qt::transparent );
    
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    if( running ) {
      
      if( fading_mode_ & FADE_FIRST ) 
      {
        painter.setOpacity( double(frame)/timeLine().startFrame() );
        painter.drawPixmap( QPoint(0,0), first_ );
      }
      
      if( fading_mode_ & FADE_SECOND ) 
      {
        painter.setOpacity( 1.0 - double(frame)/timeLine().startFrame() );
        painter.drawPixmap( QPoint(0,0), second_ );
      }
      
      painter.end();
      
    } else painter.drawPixmap( QPoint(0,0), first_ );
  }
  
  if( running && frame <= 0 ) { timeLine().stop(); }
  
}

//___________________________________________________________________
void TransitionWidget::_updateConfiguration( void )
{
  Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );
  int duration( XmlOptions::get().get<int>( "ANIMATION_DURATION" ) );
  setEnabled( duration > 0 );
  if( enabled() ) timeLine().setDuration( duration );
  timeLine().setFrameRange( XmlOptions::get().get<int>( "ANIMATION_FRAMES" ), 0 );
} 

//___________________________________________________________________
TransitionWidget::Pixmap::Pixmap( void ):
  Counter( "TransitionWidget::Pixmap" )
  {}
  
//___________________________________________________________________
TransitionWidget::Pixmap::Pixmap( QSize size ):
  QPixmap( size ),
  Counter( "TransitionWidget::Pixmap" )
{ 
  Debug::Throw( "TransitionWidget::Pixmap::Pixmap.\n" );
  assert( size.isValid() );
  fill( Qt::transparent ); 
}
 
//___________________________________________________________________
TransitionWidget::Pixmap::Pixmap( const QPixmap& pixmap ):
  QPixmap( pixmap ),
  Counter( "TransitionWidget::Pixmap" )
{}

//___________________________________________________________________
void TransitionWidget::Pixmap::fromWidget( QWidget* parent )
{
  
  Debug::Throw( "TransitionWidget::Pixmap::fromWidget.\n" );
  if( isNull() ) return;
  
  QPainter painter( this );
  painter.setRenderHints(QPainter::SmoothPixmapTransform);
  if( !( parent->isVisible() || size() == parent->size() ) ) parent->resize( size() );
  
  // draw widget children (and not the parent)
  parent->render( &painter, QPoint( 0, 0 ), QRegion(), QWidget::DrawChildren );
  painter.end();
  
}

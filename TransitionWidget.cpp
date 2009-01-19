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
  mode_( FADE_BOTH )
{
  Debug::Throw( "TransitionWidget::TransitionWidget.\n" );
  connect( &timeLine(), SIGNAL(frameChanged(int)), this, SLOT(update())); 
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//___________________________________________________________________
void TransitionWidget::resize( const QSize& size )
{
  Debug::Throw( "TransitionWidget::resize.\n" );
  first_ = Pixmap( size );
  second_ = Pixmap( size );
  QWidget::resize( size );
}

//___________________________________________________________________
void TransitionWidget::setStartWidget( QWidget* widget )
{ 
  Debug::Throw( "TransitionWidget::setStartWidget.\n" );
  first_.fromWidget( widget ); 
}


//___________________________________________________________________
void TransitionWidget::setEndWidget( QWidget* widget )
{ 
  Debug::Throw( "TransitionWidget::setEndWidget.\n" );
  second_.fromWidget( widget ); 
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
  if( mode_ != NONE )
  {
    QPainter painter( this );
    painter.fillRect( rect(), Qt::transparent );
    
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    if( running ) {
      
      if( mode_ & FADE_FIRST ) painter.setOpacity( frame/1000 );
      painter.drawPixmap( QPoint(0,0), first_ );
    
      if( mode_ & FADE_SECOND ) painter.setOpacity( 1.0 - frame/1000 );
      painter.drawPixmap( QPoint(0,0), second_ );
      painter.end();
      
    } else painter.drawPixmap( QPoint(0,0), first_ );
  }
  
  if( running && frame <= 0 ) { timeLine().stop(); }
  
}

//___________________________________________________________________
void TransitionWidget::_updateConfiguration( void )
{
  Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );
  timeLine().setDuration( XmlOptions::get().get<int>( "ANIMATION_DURATION" ) );
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
void TransitionWidget::Pixmap::fromWidget( QWidget* parent )
{
  
  Debug::Throw( "TransitionWidget::Pixmap::fromWidget.\n" );
  if( isNull() ) return;
  
  QPainter painter( this );
  painter.setRenderHints(QPainter::SmoothPixmapTransform);
  if( !( parent->isVisible() || size() == parent->size() ) ) parent->resize( size() );
  qApp->processEvents();
  
  // draw widget children (and not the parent)
  parent->render( &painter, QPoint( 0, 0 ), QRegion(), QWidget::DrawChildren );
  painter.end();
  
}

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
  \file AnimatedStackedWidget.cpp
  \brief animated stacked widget
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>

#include "AnimatedStackedWidget.h"
#include "Debug.h"

using namespace std;
//___________________________________________________________________
Fader::Fader( QWidget *parent ):
  QWidget( parent ),
  Counter( "Fader" ),
  time_line_( 300, this )
{
  timeLine().setFrameRange(1000, 0);  
  connect( &timeLine(), SIGNAL(frameChanged(int)), this, SLOT(update())); 
}

//___________________________________________________________________
void Fader::start( QSize size, QWidget* first, QWidget* second )
{
  resize( size );
  first_pixmap_ = QPixmap( size );  
  first_pixmap_.fill( Qt::transparent );
  {
    QPainter painter( &first_pixmap_ );
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    first->render( &painter, QPoint( 0, 0 ), QRegion(), QWidget::DrawChildren );
    painter.end();
  }

  second_pixmap_ = QPixmap( size );
  second_pixmap_.fill( Qt::transparent );
  {
    QPainter painter( &second_pixmap_ );
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    second->resize( size );
    second->render( &painter, QPoint( 0, 0 ), QRegion(), QWidget::DrawChildren );
    painter.end();
  }

  if( timeLine().state() != QTimeLine::Running ) timeLine().start();
  
}

//___________________________________________________________________
void Fader::paintEvent( QPaintEvent* event )
{
  
  qreal frame = timeLine().currentFrame();
  if( timeLine().state() != QTimeLine::Running ) 
  { return; }
  
  QPainter painter( this );
  painter.fillRect( rect(), Qt::transparent );
  
  painter.setRenderHints(QPainter::SmoothPixmapTransform);
  painter.setOpacity( frame/1000 );
  painter.drawPixmap( QPoint(0,0), first_pixmap_ );
  
  painter.setOpacity( 1.0 - frame/1000 );
  painter.drawPixmap( QPoint(0,0), second_pixmap_ );
  painter.end();
  
  if( frame <= 0 ) { timeLine().stop(); }
}

//___________________________________________________________________
AnimatedStackedWidget::AnimatedStackedWidget( QWidget* parent ):
  QStackedWidget( parent ),
  Counter( "AnimatedStackedWidget" ),
  widget_( 0 ),
  fader_( new Fader( this ) )
{
  Debug::Throw( "AnimatedStackedWidget::AnimatedStackedWidget.\n" );
 
  { setAttribute(Qt::WA_NoSystemBackground); }
                                              
  addWidget( &_fader() );
  connect( &_fader().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );
}

//______________________________________________________________
AnimatedStackedWidget::~AnimatedStackedWidget( void )
{
  Debug::Throw( "AnimatedStackedWidget::~AnimatedStackedWidget.\n" );
  return;
}

//___________________________________________________________________
void AnimatedStackedWidget::setCurrentIndex( int index )
{ 
  Debug::Throw( "AnimatedStackedWidget::setCurrentIndex.\n" );
  QWidget* widget =  AnimatedStackedWidget::widget( index );
  if( !widget ) QStackedWidget::setCurrentIndex( index );
  else setCurrentWidget( widget );
}

//___________________________________________________________________
void AnimatedStackedWidget::setCurrentWidget( QWidget* widget )
{
  
  // check widget validity
  if( !( widget && indexOf( widget ) >= 0 ) ) return QStackedWidget::setCurrentWidget( widget ); 
  
  // check if there is already a current widget
  if( !currentWidget() ) return QStackedWidget::setCurrentWidget( widget ); 
  
  // store current widget
  widget_ = widget;

  // start fader
  _fader().start( size(), currentWidget(), widget_ );
  QStackedWidget::setCurrentWidget( &_fader() );
 // _fader().show();
  
}

//___________________________________________________________________
void AnimatedStackedWidget::_animationFinished( void )
{
  _fader().hide();
  QStackedWidget::setCurrentWidget( widget_ );
}

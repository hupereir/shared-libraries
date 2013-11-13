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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

/*!
  \file AnimatedTabWidget.cpp
  \brief animated stacked widget
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>
#include <QTabBar>

#include "AnimatedTabWidget.h"
#include "AnimatedTabWidget.moc"
#include "Debug.h"
#include "TransitionWidget.h"



//___________________________________________________________________
void AnimatedTabBar::mousePressEvent( QMouseEvent* event )
{
  Debug::Throw( "AnimatedTabWidget::mousePressEvent.\n" );
  if( event->button() == Qt::LeftButton ) emit clicked();
  QTabBar::mousePressEvent( event );
}

//___________________________________________________________________
AnimatedTabWidget::AnimatedTabWidget( QWidget* parent ):
  QTabWidget( parent ),
  Counter( "AnimatedTabWidget" ),
  transitionWidget_( new TransitionWidget( this ) )
{
  Debug::Throw( "AnimatedTabWidget::AnimatedTabWidget.\n" );

  setTabBar( new AnimatedTabBar( this ) );

  _transitionWidget().setFlag( TransitionWidget::SHOW, false );
  _transitionWidget().hide();
  connect( tabBar(), SIGNAL( clicked() ), SLOT( _updateCurrentWidget() ) );
  connect( tabBar(), SIGNAL( currentChanged( int ) ), SLOT( _startAnimation() ) );
  connect( &_transitionWidget().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );

}

//______________________________________________________________
AnimatedTabWidget::~AnimatedTabWidget( void )
{
  Debug::Throw( "AnimatedTabWidget::~AnimatedTabWidget.\n" );
  return;
}

//___________________________________________________________________
void AnimatedTabWidget::_updateCurrentWidget( void )
{
  Debug::Throw( "AnimatedTabWidget::_updateCurrentWidget.\n" );

  // check enability
  if( !( _transitionWidget().isEnabled() && isVisible() ) ) return;

  // check
  QWidget *widget( currentWidget() );
  if( !widget ) return;
  _transitionWidget().initialize( widget );

}

//___________________________________________________________________
void AnimatedTabWidget::_startAnimation( void )
{

  Debug::Throw( "AnimatedTabWidget::_startAnimation.\n" );

  if( !( _transitionWidget().isEnabled() && isVisible() ) ) return;
  _transitionWidget().setParent( currentWidget() );
  _transitionWidget().show();
  _transitionWidget().start();

}

//___________________________________________________________________
void AnimatedTabWidget::_animationFinished( void )
{
  _transitionWidget().setParent( this );
  _transitionWidget().hide();
}

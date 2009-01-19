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
AnimatedStackedWidget::AnimatedStackedWidget( QWidget* parent ):
  QStackedWidget( parent ),
  Counter( "AnimatedStackedWidget" ),
  widget_( 0 ),
  transition_widget_( new TransitionWidget() )
{
  Debug::Throw( "AnimatedStackedWidget::AnimatedStackedWidget.\n" );
  addWidget( &_transitionWidget() );
  connect( &_transitionWidget().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );
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

  // start transitionWidget
  _transitionWidget().start( size(), currentWidget(), widget_ );
  QStackedWidget::setCurrentWidget( &_transitionWidget() );
  
}

//___________________________________________________________________
void AnimatedStackedWidget::_animationFinished( void )
{ QStackedWidget::setCurrentWidget( widget_ ); }

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
#include "TransitionWidget.h"

using namespace std;

//___________________________________________________________________
AnimatedStackedWidget::AnimatedStackedWidget( QWidget* parent ):
  QStackedWidget( parent ),
  Counter( "AnimatedStackedWidget" ),
  transition_widget_( new TransitionWidget( this ) )
{
  Debug::Throw( "AnimatedStackedWidget::AnimatedStackedWidget.\n" );
  _transitionWidget().hide();
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
  
  // check animation time
  if( !_transitionWidget().isEnabled() ) return QStackedWidget::setCurrentIndex( index );
  
  // check index is changed
  if( index == currentIndex() ) return QStackedWidget::setCurrentIndex( index );
  
  // pass to setCurrentWidget method
  setCurrentWidget( AnimatedStackedWidget::widget( index ) );
}

//___________________________________________________________________
void AnimatedStackedWidget::setCurrentWidget( QWidget* widget )
{

  // check index is changed
  if( widget == currentWidget() || !widget ) return QStackedWidget::setCurrentWidget( widget );
    
  // check widget validity and animation time
  if( !(  _transitionWidget().isEnabled() && isVisible() ) ) return QStackedWidget::setCurrentWidget( widget );

  // start _transitionWidget
  _transitionWidget().resize( size() );
  _transitionWidget().setStartWidget( this, QRect(), true );
  _transitionWidget().setParent( widget );
  _transitionWidget().show();
  
  QStackedWidget::setCurrentWidget( widget );
  _transitionWidget().start();
  
}

//___________________________________________________________________
void AnimatedStackedWidget::_animationFinished( void )
{ 
  _transitionWidget().setParent( this );
  _transitionWidget().hide();
}

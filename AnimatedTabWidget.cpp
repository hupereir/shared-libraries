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
  \file AnimatedTabWidget.cpp
  \brief animated stacked widget
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>

#include "AnimatedTabWidget.h"
#include "Debug.h"

using namespace std;

//___________________________________________________________________
AnimatedTabWidget::AnimatedTabWidget( QWidget* parent ):
  QTabWidget( parent ),
  Counter( "AnimatedTabWidget" ),
  transition_widget_( new TransitionWidget() )
{
  Debug::Throw( "AnimatedTabWidget::AnimatedTabWidget.\n" );
  connect( &transitionWidget().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );
}

//______________________________________________________________
AnimatedTabWidget::~AnimatedTabWidget( void )
{
  Debug::Throw( "AnimatedTabWidget::~AnimatedTabWidget.\n" );
  return;
}

//___________________________________________________________________
void AnimatedTabWidget::setCurrentIndex( int index )
{ 
  Debug::Throw( "AnimatedTabWidget::setCurrentIndex.\n" );
  
  // check animation time
  if( !transitionWidget().enabled() ) 
  { return QTabWidget::setCurrentIndex( index ); }
  
  // check index is changed
  if( index == currentIndex() ) return QTabWidget::setCurrentIndex( index );
  
  // check widget
  QWidget* widget =  AnimatedTabWidget::widget( index );
  if( !widget ) QTabWidget::setCurrentIndex( index );
  else setCurrentWidget( widget );
}

//___________________________________________________________________
void AnimatedTabWidget::setCurrentWidget( QWidget* widget )
{
  
  // check widget validity and animation time
  if( !(  transitionWidget().enabled() && widget && indexOf( widget ) >= 0 ) )
  { return QTabWidget::setCurrentWidget( widget ); } 

  // check index is changed
  if( widget == currentWidget() ) return QTabWidget::setCurrentWidget( widget );

  // check index is changed
  if( !isVisible() ) return QTabWidget::setCurrentWidget( widget );
  
  // check if there is already a current widget
  if( !currentWidget() ) return QTabWidget::setCurrentWidget( widget ); 
  
  transitionWidget().resize( currentWidget()->size() );
  transitionWidget().setStartWidget( currentWidget() );
  transitionWidget().setParent( widget );
  transitionWidget().show();
  setUpdatesEnabled( false );
  QTabWidget::setCurrentWidget( widget );
  transitionWidget().setEndWidget( widget );
  transitionWidget().start();
  setUpdatesEnabled( true );
  
}

//___________________________________________________________________
void AnimatedTabWidget::_animationFinished( void )
{ 
  transitionWidget().setParent( this );
  transitionWidget().hide();
}

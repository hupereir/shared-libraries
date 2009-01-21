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
#include <QTabBar>

#include "AnimatedTabWidget.h"
#include "Debug.h"
#include "TransitionWidget.h"

using namespace std;

//___________________________________________________________________
AnimatedTabWidget::AnimatedTabWidget( QWidget* parent ):
  QTabWidget( parent ),
  Counter( "AnimatedTabWidget" ),
  previous_widget_( 0 ),
  transition_widget_( new TransitionWidget( this ) )
{
  Debug::Throw( "AnimatedTabWidget::AnimatedTabWidget.\n" );
  transitionWidget().setFadingMode( TransitionWidget::FADE_FIRST );
  transitionWidget().setCopyMode( TransitionWidget::RENDER );
  
  connect( tabBar(), SIGNAL( currentChanged( int ) ), SLOT( _updateCurrentWidget( int ) ) );  
  connect( &transitionWidget().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );
}

//______________________________________________________________
AnimatedTabWidget::~AnimatedTabWidget( void )
{
  Debug::Throw( "AnimatedTabWidget::~AnimatedTabWidget.\n" );
  return;
}

//___________________________________________________________________
void AnimatedTabWidget::_updateCurrentWidget( int index )
{
  Debug::Throw( "AnimatedTabWidget::_updateCurrentWidget.\n" );
  
  // check enability
  if( !transitionWidget().enabled() ) return;
  
  QWidget* widget( QTabWidget::widget( index ) );
  transitionWidget().resize( widget->size() );
  transitionWidget().setParent( widget );
  if( previous_widget_ ) transitionWidget().setStartWidget( previous_widget_ );
  transitionWidget().show();
  transitionWidget().start();
  
}

//___________________________________________________________________
void AnimatedTabWidget::_animationFinished( void )
{ 
  transitionWidget().setParent( this );
  transitionWidget().hide();
  previous_widget_ = currentWidget();  
}

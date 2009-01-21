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
  
  _transitionWidget().hide();
  connect( tabBar(), SIGNAL( currentChanged( int ) ), SLOT( _updateCurrentWidget( int ) ) );  
  connect( &_transitionWidget().timeLine(), SIGNAL( finished() ), SLOT( _animationFinished() ) );
  
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
  if( !( _transitionWidget().enabled() && isVisible() ) ) 
  {
    previous_widget_ = widget( index );  
    return;
  }
  
  QWidget* widget( QTabWidget::widget( index ) );
  _transitionWidget().resize( widget->size() );
  _transitionWidget().setParent( widget );
  if( previous_widget_ ) _transitionWidget().setStartWidget( previous_widget_ );
  _transitionWidget().show();
  _transitionWidget().start();
  
}

//___________________________________________________________________
void AnimatedTabWidget::_animationFinished( void )
{ 
  _transitionWidget().setParent( this );
  _transitionWidget().hide();
  previous_widget_ = currentWidget();  
}

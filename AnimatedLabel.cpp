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

#include "AnimatedLabel.h"
#include "Debug.h"
#include "TransitionWidget.h"

#include <QApplication>

//________________________________________________________
AnimatedLabel::AnimatedLabel( QWidget* parent ):
  QLabel( parent ),
  Counter( "AnimatedLabel" ),
  transition_widget_( new TransitionWidget(this) )
{
  Debug::Throw( "AnimatedLabel::AnimatedLabel.\n" );
  transitionWidget().setFlag( TransitionWidget::FromParent, false );
  transitionWidget().hide();
  connect( &transitionWidget().timeLine(), SIGNAL(finished()),  &transitionWidget(), SLOT(hide()) );
}

//________________________________________________________
AnimatedLabel::~AnimatedLabel( void )
{ Debug::Throw( "AnimatedLabel::~AnimatedLabel.\n" ); }

//________________________________________________________
void AnimatedLabel::setText( const QString& text )
{

  // check enability
  if( !( transitionWidget().isEnabled() && isVisible() ) ) return QLabel::setText( text );
  else {
    transitionWidget().initialize();
    QLabel::setText( text );
    transitionWidget().start();

  }

}

//________________________________________________________
void AnimatedLabel::clear( void )
{
  // check enability
  if( !( transitionWidget().isEnabled() && isVisible() ) ) return QLabel::clear();
  else {

    transitionWidget().initialize();
    QLabel::clear();
    transitionWidget().start();

  }

}

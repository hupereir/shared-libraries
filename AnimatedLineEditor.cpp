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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software , Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file AnimatedLineEditor.cpp
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "AnimatedLineEditor.h"
#include "Debug.h"
#include "TransitionWidget.h"

using namespace std;

//________________________________________________________
AnimatedLineEditor::AnimatedLineEditor( QWidget* parent ):
  LineEditor( parent ),
  transition_widget_( new TransitionWidget(this) )
{
  Debug::Throw( "AnimatedLineEditor::AnimatedLineEditor.\n" );
  _transitionWidget().hide();
  connect( &_transitionWidget().timeLine(), SIGNAL( finished() ),  &_transitionWidget(), SLOT( hide() ) );
}

//________________________________________________________
AnimatedLineEditor::~AnimatedLineEditor( void )
{ Debug::Throw( "AnimatedLineEditor::~AnimatedLineEditor.\n" ); }

//________________________________________________________
void AnimatedLineEditor::setText( const QString& text )
{
  
  // check enability
  if( !( _transitionWidget().isEnabled() && isVisible() ) ) return LineEditor::setText( text );
  
  _transitionWidget().resize( size() );
  _transitionWidget().setStartWidget( this, QRect(), true );
  _transitionWidget().show();
  
  // setup animation between old and new text
  LineEditor::setText( text );
  _transitionWidget().start();

}

//________________________________________________________
void AnimatedLineEditor::clear( void )
{
  // check enability
  if( !( _transitionWidget().isEnabled() && isVisible() ) ) return LineEditor::clear();

  // setup animation between old and new text
  _transitionWidget().resize( size() );
  _transitionWidget().setStartWidget( this );
  _transitionWidget().show();

  LineEditor::clear();
  _transitionWidget().start();

}

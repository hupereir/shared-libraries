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

#include "AnimatedTextEditor.h"
#include "AnimatedTextEditor.moc"

#include "Debug.h"
#include "ScrollObject.h"
#include "TransitionWidget.h"

//________________________________________________________
AnimatedTextEditor::AnimatedTextEditor( QWidget* parent ):
TextEditor( parent ),
transitionWidget_( new TransitionWidget(this) )
{
    Debug::Throw( "AnimatedTextEditor::AnimatedTextEditor.\n" );
    _transitionWidget().setFlag( TransitionWidget::FromParent, false );
    _transitionWidget().hide();
    connect( &_transitionWidget().timeLine(), SIGNAL(finished()),  &_transitionWidget(), SLOT(hide()) );

    new ScrollObject( this );

}

//________________________________________________________
AnimatedTextEditor::~AnimatedTextEditor( void )
{ Debug::Throw( "AnimatedTextEditor::~AnimatedTextEditor.\n" ); }

//________________________________________________________
void AnimatedTextEditor::setPlainText( const QString& text )
{

    // check enability
    if( !( _transitionWidget().isEnabled() && isVisible() ) ) return TextEditor::setPlainText( text );
    else {
        _transitionWidget().initialize();
        TextEditor::setPlainText( text );
        _transitionWidget().start();
    }

}

//________________________________________________________
void AnimatedTextEditor::setHtml( const QString& text )
{
    // check enability
    if( !( _transitionWidget().isEnabled() && isVisible() ) ) return TextEditor::setHtml( text );
    else {
        _transitionWidget().initialize( this );
        TextEditor::setHtml( text );
        _transitionWidget().start();
    }
}

//________________________________________________________
void AnimatedTextEditor::clear( void )
{
    // check enability
    if( !( _transitionWidget().isEnabled() && isVisible() ) ) return TextEditor::clear();
    else {
        _transitionWidget().initialize();
        TextEditor::clear();
        _transitionWidget().start();
    }

}

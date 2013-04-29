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

#include "AnimatedLineEditor.h"
#include "Debug.h"
#include "Singleton.h"
#include "TransitionWidget.h"
#include "XmlOptions.h"

#include <QPainter>

//________________________________________________________
AnimatedLineEditor::AnimatedLineEditor( QWidget* parent ):
LineEditor( parent ),
transition_widget_( new TransitionWidget(this) )
{
    Debug::Throw( "AnimatedLineEditor::AnimatedLineEditor.\n" );
    _transitionWidget().setFlag( TransitionWidget::FROM_PARENT, false );
    _transitionWidget().hide();

    connect( &_transitionWidget().timeLine(), SIGNAL( finished() ),  &_transitionWidget(), SLOT( hide() ) );
    connect( &timeLine(), SIGNAL( frameChanged( int ) ), this, SLOT( update( void )) );

    if( Singleton::get().hasApplication() )
    { connect( Singleton::get().application(), SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) ); }

    _updateConfiguration();

}

//________________________________________________________
AnimatedLineEditor::~AnimatedLineEditor( void )
{ Debug::Throw( "AnimatedLineEditor::~AnimatedLineEditor.\n" ); }

//________________________________________________________
void AnimatedLineEditor::setText( const QString& text )
{

    // check enability
    if( !( _transitionWidget().isEnabled() && isVisible() ) ) return LineEditor::setText( text );
    else {

        _transitionWidget().initialize();
        LineEditor::setText( text );
        _transitionWidget().start();

    }

}

//________________________________________________________
void AnimatedLineEditor::clear( void )
{

    // check enability
    if( !( _transitionWidget().isEnabled() && isVisible() && timeLine().state() == QTimeLine::NotRunning ) ) return LineEditor::clear();
    else {

        _transitionWidget().initialize();
        LineEditor::clear();
        _transitionWidget().start();
    }

}

//___________________________________________________________________
void AnimatedLineEditor::_updateConfiguration( void )
{

    Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );
    timeLine().setDuration( XmlOptions::get().get<int>( "SMOOTH_TRANSITION_DURATION" ) );
    timeLine().setFrameRange( 0, XmlOptions::get().get<int>( "ANIMATION_FRAMES" ) );

}

//________________________________________________
bool AnimatedLineEditor::_toggleClearButton( const bool& value )
{
    if( !LineEditor::_toggleClearButton( value ) ) return false;

    // check various cases where timeline should not be started
    if( _transitionWidget().timeLine().state() != QTimeLine::NotRunning ) return true;
    if( !(isVisible() && _transitionWidget().isEnabled() ) ) return true;
    if( timeLine().state() != QTimeLine::NotRunning ) return true;

    // start timeline
    timeLine().start();

    return true;
}

//________________________________________________
void AnimatedLineEditor::_paintClearButton( QPainter& painter )
{

    // if time line is not running, run normal paint
    if( timeLine().state() != QTimeLine::Running ) return LineEditor::_paintClearButton( painter );

    // calculate opacity
    qreal frame = timeLine().currentFrame();
    if( _clearButtonVisible() ) painter.setOpacity( frame/timeLine().endFrame() );
    else  painter.setOpacity( 1-frame/timeLine().endFrame() );
    LineEditor::_paintClearButton( painter, false );
    painter.setOpacity( 1 );

}

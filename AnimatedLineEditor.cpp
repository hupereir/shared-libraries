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
transitionWidget_( new TransitionWidget(this) )
{
    Debug::Throw( "AnimatedLineEditor::AnimatedLineEditor.\n" );
    transitionWidget_->setFlag( TransitionWidget::FromParent, false );
    transitionWidget_->hide();

    connect( &transitionWidget_->timeLine(), SIGNAL(finished()),  transitionWidget_, SLOT(hide()) );
    connect( &timeLine_, SIGNAL(frameChanged(int)), this, SLOT(update()) );

    if( Singleton::get().hasApplication() )
    { connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) ); }

    _updateConfiguration();

}

//________________________________________________________
AnimatedLineEditor::~AnimatedLineEditor( void )
{ Debug::Throw( "AnimatedLineEditor::~AnimatedLineEditor.\n" ); }

//________________________________________________________
void AnimatedLineEditor::setText( const QString& text )
{

    // check enability
    if( !( transitionWidget_->isEnabled() && isVisible() ) ) return LineEditor::setText( text );
    else {

        transitionWidget_->initialize();
        LineEditor::setText( text );
        transitionWidget_->start();

    }

}

//________________________________________________________
void AnimatedLineEditor::clear( void )
{

    // check enability
    if( !( transitionWidget_->isEnabled() && isVisible() && timeLine_.state() == QTimeLine::NotRunning ) ) return LineEditor::clear();
    else {

        transitionWidget_->initialize();
        LineEditor::clear();
        transitionWidget_->start();
    }

}

//___________________________________________________________________
void AnimatedLineEditor::_updateConfiguration( void )
{

    Debug::Throw( "TransitionWidget::_updateConfiguration.\n" );
    timeLine_.setDuration( XmlOptions::get().get<int>( "SMOOTH_TRANSITION_DURATION" ) );
    timeLine_.setFrameRange( 0, XmlOptions::get().get<int>( "ANIMATION_FRAMES" ) );

}

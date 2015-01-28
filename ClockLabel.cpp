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

#include "ClockLabel.h"
#include "Debug.h"
#include "TransitionWidget.h"

//__________________________________________________________
//! timestamp string format
static const TimeStamp::Format format( TimeStamp::JobTag );

//__________________________________________________________
ClockTimer::ClockTimer( QWidget *parent ):
    QObject( parent ),
    Counter( "ClockTimer" )
{
    Debug::Throw( "ClockTimer::ClockTimer.\n" );
    timer_.start( 1000*interval(), this );
}

//__________________________________________________________
void ClockTimer::timerEvent( QTimerEvent* event )
{

    Debug::Throw( "ClockTimer::timerEvent.\n" );
    TimeStamp newTime( TimeStamp::now() );

    if( event->timerId() == timer_.timerId() )
    {

        time_ = TimeStamp::now();
        timer_.start( 1000 * interval(), this );
        emit timeChanged( time_.toString( format ) );

    } else return QObject::timerEvent( event );

}

//________________________________________________________________
ClockLabel::ClockLabel( QWidget* parent ):
    AnimatedLabel( parent ),
    timer_( this )
{

    Debug::Throw( "ClockLabel::ClockLabel.\n" );

    // needed for proper background to be set
    transitionWidget().setFlag( TransitionWidget::FromParent, true );

    // create static clock timer, updated every 10 seconds
    connect( &timer_, SIGNAL(timeChanged(QString)), SLOT(setText(QString)) );

    // initialize
    setText( TimeStamp::now().toString( format ) );

}

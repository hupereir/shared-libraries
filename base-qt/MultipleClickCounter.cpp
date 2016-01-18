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

#include "MultipleClickCounter.h"

//______________________________________________________________________________
MultipleClickCounter::MultipleClickCounter( QObject* parent, int maxCount ):
    QObject( parent ),
    Counter( "MultipleClickCounter" ),
    maxCount_( maxCount )
{}

//______________________________________________________________________________
void MultipleClickCounter::increment( int position )
{
    Debug::Throw() << "MultipleClickCounter::increment - count_: " << count_ << endl;

    // restart timer
    timer_.start( QApplication::doubleClickInterval(), this );

    // increment counts
    if( position_ != position || toReset_ )
    {

        // reset
        toReset_ = false;
        position_ = position;
        count_ = 1;

    } else {

        // same position in text
        // increment, check against maximum
        count_++;
        if( count_ > maxCount_ ) count_ = 1;

    }

    return;

}

//______________________________________________________________________________
void MultipleClickCounter::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {
        Debug::Throw() << "MultipleClickCounter::timerEvent" << endl;
        timer_.stop();
        toReset_ = true;
    } else return QObject::timerEvent( event );
}

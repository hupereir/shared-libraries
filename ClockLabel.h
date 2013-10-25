#ifndef ClockLabel_h
#define ClockLabel_h

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

#include <QDateTime>
#include <QBasicTimer>

#include "AnimatedLabel.h"
#include "Counter.h"
#include "TimeStamp.h"

//! clock timer. Emit signal when current time is changed
class ClockTimer: public QObject, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    ClockTimer( QWidget *parent );

    Q_SIGNALS:

    //! emmited every time current time is changed
    void timeChanged( const QString& );

    protected:

    virtual void timerEvent( QTimerEvent* );

    //! check current time, generate time string if new; emit TimeChanged
    //void _checkCurrentTime( void );

    private:

    //! get interval (seconds) prior to next update
    int interval( void ) const
    { return 60 - (TimeStamp::now() % 60); }

    //! timer
    QBasicTimer timer_;

    //! current time
    TimeStamp time_;

};

//! self-updated label displaying current date and time
class ClockLabel:public AnimatedLabel
{

    public:

    //! constructor
    ClockLabel( QWidget* parent );

    //! retrieve timer
    ClockTimer& timer( void )
    { return timer_; }

    private:

    //! static timer
    ClockTimer timer_;

};


#endif

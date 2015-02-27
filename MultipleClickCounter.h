#ifndef MultipleClickCounter_h
#define MultipleClickCounter_h

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

#include <QApplication>
#include <QBasicTimer>

#include "Counter.h"
#include "Debug.h"

//! handles multiple clicks and timeout
class MultipleClickCounter: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    MultipleClickCounter( QObject*, int maxCount );

    //! destructor
    virtual ~MultipleClickCounter( void )
    { Debug::Throw( "MultipleClickCounter::~MultipleClickCounter.\n" ); }

    //! increment counter and return current value
    void increment( const int& position  = 0 );

    //! returns current counts
    const unsigned int& counts( void ) const
    { return count_; }

    protected:

    //! timerEvent
    virtual void timerEvent( QTimerEvent* );

    private:

    //! timer
    QBasicTimer timer_;

    //! true if need to reset at next increment
    bool toReset_;

    //! last click position in text
    int position_;

    //! max count
    unsigned int maxCount_;

    //! current number of clicks
    unsigned int count_;

};

#endif

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

#include "Counter.h"
#include "Debug.h"
#include "base_qt_export.h"
#include <QApplication>
#include <QBasicTimer>


//* handles multiple clicks and timeout
class BASE_QT_EXPORT MultipleClickCounter: public QObject, private Base::Counter<MultipleClickCounter>
{

    Q_OBJECT

    public:

    //* constructor
    explicit MultipleClickCounter( QObject*, int maxCount );

    //* increment counter and return current value
    void increment( int = 0 );

    //* returns current counts
    int counts() const
    { return count_; }

    protected:

    //* timerEvent
    void timerEvent( QTimerEvent* ) override;

    private:

    //* timer
    QBasicTimer timer_;

    //* true if need to reset at next increment
    bool toReset_ = true;

    //* last click position in text
    int position_ = 0;

    //* max count
    int maxCount_ = 1;

    //* current number of clicks
    int count_ = 0;

};

#endif

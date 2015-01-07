#ifndef WidgetDragMonitor_h
#define WidgetDragMonitor_h

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

#include "Counter.h"
#include "MultipleClickCounter.h"

#include <QBasicTimer>
#include <QSize>
#include <QTimerEvent>
#include <QWidget>

//* used to automatically save size of top level widgets
class WidgetDragMonitor: public QObject, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    WidgetDragMonitor( QWidget* );

    //* destructor
    virtual ~WidgetDragMonitor() = default;

    //*@name accessors
    //@{

    //* enabled state
    bool isEnabled( void ) const
    { return enabled_; }

    //@}

    //* event filter
    virtual bool eventFilter( QObject*, QEvent* );

    Q_SIGNALS:

    void stateChangeRequest( void );

    public Q_SLOTS:

    void toggleState( void )
    { enabled_ = !enabled_; }

    void setEnabled( bool value )
    { enabled_ = value; }

    protected:

    //* timer event
    virtual void timerEvent( QTimerEvent* );

    protected Q_SLOTS:

    //* start drag
    bool _startDrag( void );

    //* reset drag
    void _resetDrag( void );

    private:

    //* multiple click counter
    MultipleClickCounter clickCounter_;

    //* button state
    Qt::MouseButton button_;

    //* resize timer
    QBasicTimer timer_;

    //* dragged widget
    QWidget* target_;

    //* click position
    QPoint dragPosition_;

    //* true when move is enabled
    bool isDragging_;

    //* enabled state
    bool enabled_;

};

#endif

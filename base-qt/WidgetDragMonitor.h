#ifndef WidgetDragMonitor_h
#define WidgetDragMonitor_h

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
#include "XcbDefines.h"
#include "base_qt_export.h"

#include <QBasicTimer>
#include <QSize>
#include <QTimerEvent>
#include <QWidget>

//* used to automatically save size of top level widgets
class BASE_QT_EXPORT WidgetDragMonitor: public QObject, private Base::Counter<WidgetDragMonitor>
{

    Q_OBJECT

    public:

    //* constructor
    explicit WidgetDragMonitor( QWidget* );

    //* mode
    enum ModeFlag
    {
        DragMove = 1<<0,
        DragResize = 1<<1
    };

    Q_DECLARE_FLAGS( Mode, ModeFlag );

    //*@name accessors
    //@{

    //* mode
    Mode mode() const
    { return mode_; }

    //* enabled state
    bool isEnabled() const
    { return enabled_; }

    //@}

    //*@name mode
    //@{

    //* mode
    void setMode( Mode value )
    { mode_ = value; }

    //@}

    //* event filter
    bool eventFilter( QObject*, QEvent* ) override;

    void toggleState()
    {
        enabled_ = !enabled_;
        if( !enabled_ ) _resetDrag();
    }

    void setEnabled( bool value )
    {
        enabled_ = value;
        if( !enabled_ ) _resetDrag();
    }

    Q_SIGNALS:

    void stateChangeRequest();

    protected:

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    private:

    //* start drag
    bool _startDrag();

    //* reset drag
    void _resetDrag();

    //* get corner from (relative) position
    XcbDefines::Direction _direction( QWidget* widget, QPoint  ) const;

    //* mode
    Mode mode_;

    //* enabled state
    bool enabled_;

    //* multiple click counter
    MultipleClickCounter clickCounter_;

    //* button state
    Qt::MouseButton button_;

    //* resize timer
    QBasicTimer timer_;

    //* dragged widget
    QWidget* target_;

    //* window resize direction
    XcbDefines::Direction direction_;

    //* size at click time
    QSize dragSize_;

    //* click position
    QPoint dragPosition_;

    //* true when move is enabled
    bool isDragging_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( WidgetDragMonitor::Mode );

#endif

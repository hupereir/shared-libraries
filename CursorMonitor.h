#ifndef CursorMonitor_h
#define CursorMonitor_h

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

#include <QBasicTimer>
#include <QObject>
#include <QTimerEvent>
#include <QCursor>
#include <QWidget>

//! used to automatically hide mouse cursor over a certain widget
class CursorMonitor: public QObject, public Counter
{

    public:

    //! constructor
    CursorMonitor( QWidget* );

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    //! auto hide delay_setCursorState
    void setAutoHideDelay( const int& value )
    { autoHideDelay_ = value; }

    //! enability
    void setEnabled( bool value )
    {
        enabled_ = value;
        if( !enabled_ ) _setCursorState( CursorVisible );
    }

    protected:

    //! timer event
    /* need to save updated window size */
    virtual void timerEvent( QTimerEvent* );

    enum CursorState
    {
        CursorVisible,
        CursorHidden
    };

    void _setCursorState( const CursorState& );

    private:

    //! enability
    bool enabled_;

    //! auto hide delay
    int autoHideDelay_;

    //! auto hide timer
    QBasicTimer autoHideTimer_;

    //! state
    CursorState cursorState_;

    //! cursor shape
    Qt::CursorShape savedCursorShape_;

};

#endif

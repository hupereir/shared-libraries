#ifndef CursorMonitor_h
#define CursorMonitor_h

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include <QtGui/QCursor>
#include <QtGui/QWidget>

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
    void setEnabled( const bool& value )
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

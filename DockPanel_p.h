#ifndef DockPanel_p_h
#define DockPanel_p_h

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
#include "MultipleClickCounter.h"

#include <QBasicTimer>
#include <QEvent>
#include <QFrame>

//! local widget to implement close_event of the content
class LocalWidget: public QFrame, public Counter
{

    public:

    //! constructor
    LocalWidget( QWidget* parent );

    //! update actions
    void updateActions( bool );

    //! detach action
    QAction& detachAction( void ) const
    { return *detachAction_; }

    //! stay on top
    QAction& staysOnTopAction( void ) const
    { return *staysOnTopAction_; }

    //! widget is hidden from taskBar
    QAction& stickyAction( void ) const
    { return *stickyAction_; }

    protected:

    //! closeEvent
    virtual void closeEvent( QCloseEvent* event );

    //! mouse press event [overloaded]
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseReleaseEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseMoveEvent( QMouseEvent* );

    //! timer event [overloaded]
    virtual void timerEvent( QTimerEvent* );

    //! paint event
    virtual void resizeEvent( QResizeEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! start drag
    bool _startDrag( void );

    //! reset drag
    void _resetDrag( void );

    private:

    //! actions
    void _installActions( void );

    //!@name actions
    //@{

    //! attach/detach action
    QAction* detachAction_;

    //! stay on top
    QAction* staysOnTopAction_;

    //! make window sticky
    QAction* stickyAction_;

    //@}

    //! multiple click counter
    MultipleClickCounter clickCounter_;

    //! button state
    Qt::MouseButton button_;

    //! move timer
    QBasicTimer timer_;

    //! click position
    QPoint dragPosition_;

    //! dragging
    bool isDragging_;

};

#endif

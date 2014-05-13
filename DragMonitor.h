#ifndef DragMonitor_h
#define DragMonitor_h

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

#include <QObject>
#include <QWidget>

//! used to install
class DragMonitor: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    DragMonitor( QWidget* );

    //! destructor
    virtual ~DragMonitor( void )
    {}

    //!@name accessors
    //@{

    //! enabled
    bool isDragEnabled( void ) const
    { return dragEnabled_; }

    //! in progress
    bool isDragInProgress( void ) const
    { return dragInProgress_; }

    //! position
    QPoint dragOrigin( void ) const
    { return dragOrigin_; }

    //@}

    //!@name modifiers
    //@{

    //! enabled
    void setDragEnabled( bool value )
    { dragEnabled_ = value; }

    //! enable/disable window grabbing
    void setWindowGraggingEnabled( bool );

    //@}

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    Q_SIGNALS:

    //! emitted when drag started from given position
    void dragStarted( QPoint );

    private:

    //! true if drag is enabled (false by default)
    bool dragEnabled_;

    //! drag
    bool dragInProgress_;

    //! drag position
    QPoint dragOrigin_;

};

#endif

#ifndef _ScrollObject_h_
#define _ScrollObject_h_

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

#include <QAbstractScrollArea>
#include <QTimeLine>
#include <QSet>

//! customized tree view
class ScrollObject: public QObject, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    ScrollObject( QAbstractScrollArea* );

    //! destructor
    virtual ~ScrollObject( void );

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    //! enability
    void setEnabled( bool value )
    { enabled_ = value; }

    //! enability
    const bool& isEnabled( void ) const
    { return enabled_; }

    protected:

    //! keypress event
    bool keyPressEvent( QKeyEvent* );

    //! keypress event
    bool keyReleaseEvent( QKeyEvent* );

    //! wheel event
    bool wheelEvent( QWheelEvent*, Qt::Orientation );

    //! wheel event
    bool mousePressEvent( QMouseEvent* );

    //! time line
    QTimeLine& _timeLine( void )
    { return timeLine_; }

    //! target
    QAbstractScrollArea& _target( void ) const
    { return *target_; }

    //! starting point
    void _setStart( const QPoint& value )
    { start_ = value; }

    //! starting point
    const QPoint& _start( void ) const
    { return start_; }

    //! current point
    QPoint _current( void ) const;

    //! set current point
    bool _setCurrent( QPoint ) const;

    //! end point
    void _setStep( const QPointF& value )
    { step_ = value; }

    //! end point
    const QPointF& _step( void ) const
    { return step_; }

    //! auto_repeat
    void _setAutoRepeat( bool value )
    { autoRepeat_ = value; }

    //! auto_repeat
    bool _autoRepeat( void ) const
    { return autoRepeat_; }

    protected Q_SLOTS:

    //! animated scroll
    void _scroll( int );

    //! finished
    void _finished( void );

    private Q_SLOTS:

    void _updateConfiguration( void );

    private:

    //! single step
    bool _singleStep( int, Qt::Orientations );

    //! page step
    bool _pageStep( int, Qt::Orientations );

    //! previous page
    bool _previousPage( Qt::Orientations );

    //! next page
    bool _nextPage( Qt::Orientations );

    //! scroll contents
    bool _scrollBy( QPoint );

    //! enability
    bool enabled_;

    //! scroll mode
    Qt::Orientations mode_;

    //! auto repeat
    bool autoRepeat_;

    // target
    QAbstractScrollArea* target_;

    //! set of captured objects
    typedef QSet<QObject*> ObjectSet;

    //! set of captured objects
    ObjectSet targets_;

    // smooth scrolling
    QTimeLine timeLine_;

    //! starting point
    QPoint start_;

    //! step
    QPointF step_;

};

// Q_DECLARE_OPERATORS_FOR_FLAGS( ScrollObject::Modes )

#endif

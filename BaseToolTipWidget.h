#ifndef BaseToolTipWidget_h
#define BaseToolTipWidget_h

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

#include <QBasicTimer>
#include <QTimerEvent>

#include <QPaintEvent>
#include <QWidget>

class GridLayoutItem;

class BaseToolTipWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    BaseToolTipWidget( QWidget* );

    //* destructo
    virtual ~BaseToolTipWidget( void )
    {}

    //*@name accessors
    //@{

    //* enabled
    bool isEnabled( void ) const
    { return enabled_; }

    //* follow mouse
    bool followMouse( void ) const
    { return followMouse_; }

    //* default delay
    int defaultDelay( void ) const
    { return defaultDelay_; }

    //* index rect
    const QRect& indexRect( void ) const
    { return rect_; }

    //* preferred position
    enum Position
    {
        Left,
        Right,
        Top,
        Bottom
    };

    //* preferred position
    Position preferredPosition( void ) const
    { return preferredPosition_; }

    //@}

    //*@name modifiers
    //@{

    //* enable state
    void setEnabled( bool );

    //* follow mouse
    void setFollowMouse( bool value )
    { followMouse_ = value; }

    //* default delay
    void setDefaultDelay( int value )
    { if( value >= 0 ) defaultDelay_ = value; }

    //* set index rect
    void setIndexRect( const QRect& rect )
    {
        rect_ = rect;
        if( isVisible() ) _adjustPosition();
    }

    //* preferred position
    void setPreferredPosition( Position value )
    {
        if( preferredPosition_ == value ) return;
        preferredPosition_ = value;
        if( isVisible() ) _adjustPosition();
    }

    //@}

    // event filter
    virtual bool eventFilter( QObject*, QEvent* );

    public Q_SLOTS:

    //* hide
    virtual void hide( void );

    //* show
    virtual void show( void );

    //* show delayed
    void showDelayed( int = -1 );

    protected:

    //* paint
    virtual void paintEvent( QPaintEvent* );

    //* mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //* timer event
    virtual void timerEvent( QTimerEvent* );

    //* check mouse position
    bool _checkMousePosition( void ) const;

    //* adjust position
    void _adjustPosition( void );

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

    private:

    //* enable state
    bool enabled_;

    //* follow mouse
    bool followMouse_;

    //* default tooltip delay
    int defaultDelay_;

    //* prefered tooltip position with respect to index rect
    Position preferredPosition_;

    //* index rect
    QRect rect_;

    //* timer
    QBasicTimer timer_;

    //* hidden timer
    /*! if running, showing again the tooltip is immediate, even when calling showDelayed */
    QBasicTimer hiddenTimer_;

};

#endif

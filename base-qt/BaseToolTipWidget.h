#ifndef BaseToolTipWidget_h
#define BaseToolTipWidget_h

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

class BaseToolTipWidget: public QWidget, private Base::Counter<BaseToolTipWidget>
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseToolTipWidget( QWidget* );

    //*@name accessors
    //@{

    //* enabled
    bool isEnabled() const
    { return enabled_; }

    //* follow mouse
    bool followMouse() const
    { return followMouse_; }

    //* default delay
    int defaultDelay() const
    { return defaultDelay_; }

    //* index rect
    const QRect& indexRect() const
    { return rect_; }

    //* preferred position
    enum class Position
    {
        Left,
        Right,
        Top,
        Bottom
    };

    //* preferred position
    Position preferredPosition() const
    { return preferredPosition_; }

    //@}

    //*@name modifiers
    //@{

    //* enable state
    void setEnabled( bool );

    //* follow mouse
    void setFollowMouse( bool );

    //* default delay
    void setDefaultDelay( int );

    //* set index rect
    void setIndexRect( const QRect& );

    //* preferred position
    void setPreferredPosition( Position );

    //@}

    // event filter
    bool eventFilter( QObject*, QEvent* ) override;

    //* change visibility
    void setVisible( bool ) override;

    //* show delayed
    void showDelayed( int = -1 );

    protected:

    //* enter
    void enterEvent( QEvent* ) override;

    //* leave
    void leaveEvent( QEvent* ) override;

    //* paint
    void paintEvent( QPaintEvent* ) override;

    //* mouse press
    void mousePressEvent( QMouseEvent* ) override;

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    //* check mouse position
    bool _checkMousePosition() const;

    //* adjust position
    void _adjustPosition();

    private:

    //* update configuration
    void _updateConfiguration();

    //* enable state
    bool enabled_ = false;

    //* follow mouse
    bool followMouse_ = false;

    //* default tooltip delay
    int defaultDelay_ = 1000;

    //* prefered tooltip position with respect to index rect
    Position preferredPosition_ = Position::Bottom;

    //* index rect
    QRect rect_;

    //* timer
    QBasicTimer timer_;

    //* hidden timer
    /** if running, showing again the tooltip is immediate, even when calling showDelayed */
    QBasicTimer hiddenTimer_;

};

#endif

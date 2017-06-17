#ifndef BusyWidget_h
#define BusyWidget_h

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
#include <QPropertyAnimation>
#include <QWidget>
#include <QPaintEvent>
#include <QPainterPath>

class BusyWidget: public QWidget, private Base::Counter<BusyWidget>
{

    Q_OBJECT
    Q_PROPERTY( qreal angle READ angle WRITE setAngle )
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

    public:

    //* position
    enum class Location
    {
        Center,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    //* constructor
    explicit BusyWidget( QWidget* = nullptr, Location = Location::Center );

    //*@name accessors
    //@{

    //* angle
    virtual qreal angle( void ) const
    { return angle_; }

    //* opacity
    int opacity( void ) const
    { return opacity_; }

    //* size
    QSize minimumSizeHint( void ) const override
    { return QSize( 2*radius_+border_, 2*radius_+border_ ); }

    //* size
    QSize sizeHint( void ) const override
    { return minimumSizeHint(); }

    //@}

    //*@name modifiers
    //@{

    //* delay
    virtual void setDelay( int value )
    { delay_ = value; }

    //* angle
    virtual void setAngle( qreal value )
    {
        if( angle_ == value ) return;
        angle_ = value;
        update();
    }

    //* opacity
    virtual void setOpacity( int );

    //@}

    //* event filter
    bool eventFilter( QObject*, QEvent* ) override;

    public Q_SLOTS:

    //* start
    virtual void start( void );

    //* stop
    virtual void stop( void );

    protected:

    //* paint event
    void paintEvent( QPaintEvent* ) override;

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    //* adjust position based on parent
    void _adjustPosition( void );

    private:

    //* animation
    QPropertyAnimation* animation_ = nullptr;

    //* basic time
    QBasicTimer timer_;

    //* painter path
    QPainterPath path_;

    //* location
    Location location_ = Location::Center;

    //* delay
    int delay_ = 200;

    //* radius
    int radius_ = 18;

    //* thickness
    int thickness_ = 5;

    //* border
    int border_ = 2;

    //* margin
    int margin_ = 10;

    //* angle
    qreal angle_ = 0;

    //* opacity
    int opacity_ = 0;

};

#endif

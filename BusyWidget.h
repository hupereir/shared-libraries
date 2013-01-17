#ifndef BusyWidget_h
#define BusyWidget_h

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
#include <QtCore/QTimerEvent>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QWidget>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainterPath>

class BusyWidget: public QWidget, public Counter
{

    Q_OBJECT
    Q_PROPERTY( qreal angle READ angle WRITE setAngle )
    Q_PROPERTY( qreal opacity READ opacity WRITE setOpacity )

    public:

    //! position
    enum Location
    {
        Center,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    //! constructor
    BusyWidget( QWidget* = 0, Location = Center );

    //! destructor
    virtual ~BusyWidget( void )
    {}

    //!@name accessors
    //@{

    //! angle
    virtual qreal angle( void ) const
    { return angle_; }

    //! opacity
    int opacity( void ) const
    { return opacity_; }

    //! size
    virtual QSize minimumSizeHint( void ) const
    { return QSize( 2*radius_+border_, 2*radius_+border_ ); }

    //! size
    virtual QSize sizeHint( void ) const
    { return minimumSizeHint(); }

    //@}

    //!@name modifiers
    //@{

    //! delay
    virtual void setDelay( int value )
    { delay_ = value; }

    //! angle
    virtual void setAngle( qreal value )
    {
        if( angle_ == value ) return;
        angle_ = value;
        update();
    }

    //! opacity
    virtual void setOpacity( int );

    //@}

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    public slots:

    //! start
    virtual void start( void );

    //! stop
    virtual void stop( void );

    protected:

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    //! adjust position based on parent
    void _adjustPosition( void );

    private:

    //! animation
    QPropertyAnimation* animation_;

    //! basic time
    QBasicTimer timer_;

    //! painter path
    QPainterPath path_;

    //! location
    Location location_;

    //! delay
    int delay_;

    //! radius
    int radius_;

    //! thickness
    int thickness_;

    //! border
    int border_;

    //! margin
    int margin_;

    //! angle
    qreal angle_;

    //! opacity
    int opacity_;

};

#endif

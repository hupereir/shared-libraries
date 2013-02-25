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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*******************************************************************************/

#include "Counter.h"

#include <QBasicTimer>
#include <QTimerEvent>

#include <QPaintEvent>
#include <QWidget>

class ToolTipWidgetItem;

class BaseToolTipWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    BaseToolTipWidget( QWidget* );

    //! destructo
    virtual ~BaseToolTipWidget( void )
    {}

    //! enable state
    void setEnabled( bool );

    //! follow mouse
    void setFollowMouse( bool value )
    { followMouse_ = value; }

    //! set index rect
    void setIndexRect( const QRect& rect )
    { rect_ = rect; }

    // event filter
    virtual bool eventFilter( QObject*, QEvent* );

    public slots:

    //! hide
    virtual void hide( void );

    //! show
    virtual void show( void );

    //! show delayed
    void showDelayed( int = 500 );

    protected:

    //! paint
    virtual void paintEvent( QPaintEvent* );

    //! mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    //! check mouse position
    bool _checkMousePosition( void ) const;

    //! adjust position
    void _adjustPosition( void );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! enable state
    bool enabled_;

    //! follow mouse
    bool followMouse_;

    //! index rect
    QRect rect_;

    //! timer
    QBasicTimer timer_;

};

#endif

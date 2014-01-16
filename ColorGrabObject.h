#ifndef ColorGrabObject_h
#define ColorGrabObject_h

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

#include <QDialog>
#include <QEvent>
#include <QWidget>

class ColorGrabObject: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ColorGrabObject( QWidget* );

    //! destructor
    virtual ~ColorGrabObject( void )
    {}

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    Q_SIGNALS:

    void colorSelected( QColor );

    protected Q_SLOTS:

    void _grabColor( void );

    private:

    //! select color
    void _selectColorFromMouseEvent( QMouseEvent* );

    //! clear capture
    void _clearCapture( void );

    //! window grabber
    QDialog* captureWidget_;

    //! is true when the mouse is down
    bool mouseDown_;

};

#endif

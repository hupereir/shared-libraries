// $Id$
#ifndef _ColorGrabButton_h_
#define _ColorGrabButton_h_

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

#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolButton>

//! used to pick color from screen
class ColorGrabButton : public QToolButton, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ColorGrabButton( QWidget* );

    //! destructor
    virtual ~ColorGrabButton( void )
    {}

    signals:

    void colorSelected( QString );

    protected slots:

    void _grabColor( void );

    protected:

    //! mouse press event [overloaded]
    virtual void mousePressEvent( QMouseEvent* );

    //! mouse release event [overloaded]
    virtual void mouseReleaseEvent( QMouseEvent* );

    //! mouse move event [overloaded]
    virtual void mouseMoveEvent( QMouseEvent* );

    private:

    void _selectColorFromMouseEvent( QMouseEvent* );

    //! is set to true when colorgrab is activated
    bool locked_;

    //! is true when the mouse is down
    bool mouseDown_;

};

#endif

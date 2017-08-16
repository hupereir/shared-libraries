#ifndef ColorGrabObject_h
#define ColorGrabObject_h
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

#include <memory>

class ColorGrabObject: public QObject, private Base::Counter<ColorGrabObject>
{

    Q_OBJECT

    public:

    //* constructor
    explicit ColorGrabObject( QWidget* );

    //* event filter
    bool eventFilter( QObject*, QEvent* ) override;

    Q_SIGNALS:

    void colorSelected( QColor );

    protected Q_SLOTS:

    void _grabColor();

    private:

    //* select color
    void _selectColorFromMouseEvent( QMouseEvent* );

    //* clear capture
    void _clearCapture();

    // grab deleter
    class Deleter
    {
        public:

        //* deleter
        void operator() (QDialog* dialog) const
        {
            dialog->releaseMouse();
            delete dialog;
        }

    };

    //* window grabber
    std::unique_ptr<QDialog, Deleter> captureWidget_;

    //* is true when the mouse is down
    bool mouseDown_ = false;

};

#endif

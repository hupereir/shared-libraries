#ifndef ApplicationMenu_h
#define ApplicationMenu_h

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

#include "base_qt_export.h"
#include "Counter.h"

#include <QAction>
#include <QMenu>
#include <QPointer>
#include <QWidget>

class BASE_QT_EXPORT ApplicationMenu: public QMenu, private Base::Counter<ApplicationMenu>
{

    Q_OBJECT

    public:

    //* constructor
    explicit ApplicationMenu(
        QWidget* parent = nullptr,
        QWidget* target = nullptr );

    //* set target
    void setTarget( QWidget* );

    //* update menu before showing
    void updateMenu();

    private:

    //* update from menu
    /* returns true if action was added */
    bool updateFrom( QMenu*, bool needSeparator );

    //* target menubar
    using WidgetPointer = QPointer<QWidget>;
    WidgetPointer target_;

    //* menu
    QMenu* menu_ = nullptr;

};

#endif

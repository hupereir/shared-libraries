#ifndef MenuBarAction_h
#define MenuBarAction_h

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
#include "Debug.h"

#include <QWidgetAction>
#include <QMenu>
#include <QPointer>
#include <QWidget>

class BASE_QT_EXPORT MenuBarAction: public QWidgetAction, private Base::Counter<MenuBarAction>
{

    Q_OBJECT

    public:

    //* constructor
    explicit MenuBarAction(
        QObject* parent,
        QWidget* target );

    private:

    //* update menu before showing
    void updateMenu();

    //* update from menu
    void updateFrom( QMenu*, bool needSeparator );

    //* target menubar
    using WidgetPointer = QPointer<QWidget>;
    WidgetPointer target_;

    //* menu
    QMenu* menu_ = nullptr;

};

#endif
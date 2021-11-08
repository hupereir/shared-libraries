#ifndef ToolBarMenu_h
#define ToolBarMenu_h

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
#include "base_qt_export.h"

#include <QMenu>

class ToolButtonStyleMenu;
class IconSizeMenu;

//* provides toolbar manipulation menu
class BASE_QT_EXPORT ToolBarMenu: public QMenu, private Base::Counter<ToolBarMenu>
{

    Q_OBJECT

    public:

    //* constructor
    explicit ToolBarMenu( QWidget* = nullptr );

    //*@name accessors
    //@{

    //* tool button action
    QAction& toolButtonStyleAction() const
    { return *toolButtonStyleAction_; }

    //* icons size action
    QAction& iconSizeAction() const
    { return *iconSizeAction_; }

    //* tool button style menu
    ToolButtonStyleMenu& toolButtonStyleMenu() const
    { return *toolButtonStyleMenu_; }

    //* icon size menu
    IconSizeMenu& iconSizeMenu() const
    { return *iconSizeMenu_; }

    private:

    //* tool button style
    QAction* toolButtonStyleAction_ = nullptr;
    ToolButtonStyleMenu* toolButtonStyleMenu_ = nullptr;

    //* icon size
    QAction* iconSizeAction_ = nullptr;
    IconSizeMenu* iconSizeMenu_ = nullptr;

};

#endif

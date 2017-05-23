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

#include <QMenu>

class ToolButtonStyleMenu;
class IconSizeMenu;

//* provides toolbar manipulation menu
class ToolBarMenu: public QMenu, private Base::Counter<ToolBarMenu>
{

    public:

    //* constructor
    ToolBarMenu( QWidget* = nullptr );

    //* tool button style menu
    ToolButtonStyleMenu &toolButtonStyleMenu( void ) const
    { return *toolButtonStyleMenu_; }

    //* icon size menu
    IconSizeMenu &iconSizeMenu( void ) const
    { return *iconSizeMenu_; }

    private:

    //* tool button style
    ToolButtonStyleMenu* toolButtonStyleMenu_ = nullptr;

    //* icon size
    IconSizeMenu* iconSizeMenu_ = nullptr;

};

#endif

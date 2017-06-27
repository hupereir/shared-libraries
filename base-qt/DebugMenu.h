#ifndef DebugMenu_h
#define DebugMenu_h

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

class CounterDialog;
class IconCacheDialog;

//* some basic runtime debuging
class DebugMenu: public QMenu, private Base::Counter<DebugMenu>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* flags
    enum Flag
    {
        Counters = 1<<0,
        Icons = 1<<2,
        System = 1<<3,
        Options = 1<<4,
        Default = Counters|Icons|System|Options
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //* constructor
    explicit DebugMenu( QWidget*, Flags = Flag::Default );

    //* needed to toggle visibility
    bool eventFilter( QObject*, QEvent* ) override;

    private Q_SLOTS:

    //* print object counters
    void _showCounterDialog();

    //* icon cache dialog
    void _showIconCacheDialog();

    //* show system environment
    void _showSystemEnvironment();

    //* show options
    void _showOptions();

    private:

    //* non modal Counter dialog
    CounterDialog* counterDialog_ = nullptr;

    //* non modal icon cache dialog
    IconCacheDialog* iconCacheDialog_ = nullptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( DebugMenu::Flags )

#endif

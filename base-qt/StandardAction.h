#ifndef StandardAction_h
#define StandardAction_h

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

#include <QAction>

class StandardAction: public QAction, private Base::Counter<StandardAction>
{

    Q_OBJECT

    public:

    //* action type
    enum class Type
    {
        New,
        Open,
        Save,
        SaveAs,
        Edit,
        Find,
        Replace,

        Undo,
        Redo,
        Cut,
        Copy,
        Paste,

        Info,
        Close,
        Exit
    };

    //* constructor
    explicit StandardAction( Type, QObject* );

};

#endif

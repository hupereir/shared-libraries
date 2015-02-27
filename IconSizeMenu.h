#ifndef IconSizeMenu_h
#define IconSizeMenu_h

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
#include "CustomToolButton.h"
#include "Debug.h"
#include "IconSize.h"

#include <QMenu>
#include <QHash>

//! provides icon size selection menu
class IconSizeMenu: public QMenu, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    IconSizeMenu( QWidget* parent = 0, bool custom = false );

    //! select size
    void select( IconSize::Size );

    Q_SIGNALS:

    //! emmitted when a new size is selected
    void iconSizeSelected( IconSize::Size );

    private Q_SLOTS:

    //! size selected from action
    void _selected( QAction* );

    private:

    //! action map
    using ActionMap = QHash<QAction*, IconSize::Size >;

    //! toolbar text action map
    ActionMap actions_;

};

#endif

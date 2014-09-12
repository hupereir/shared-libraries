#ifndef ToolButtonStyleMenu_h
#define ToolButtonStyleMenu_h

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
#include "Debug.h"

#include <QMenu>
#include <QMap>

//! provides tool button style selection menu
class ToolButtonStyleMenu: public QMenu, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ToolButtonStyleMenu( QWidget* parent = 0 );

    //! select size
    void select( int );

    Q_SIGNALS:

    //! emmitted when default size is selected
    void defaultStyleSelected( void );

    //! emmitted when a new size is selected
    void styleSelected( int );

    private Q_SLOTS:

    //! size selected from action
    void _selected( QAction* );

    private:

    //! action map
    using ActionMap = QMap< QAction*, int >;

    //! toolbar text action map
    ActionMap actions_;

};

#endif

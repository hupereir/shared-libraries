#ifndef ColumnSelectionMenu_h
#define ColumnSelectionMenu_h

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

#include <QAction>
#include <QMenu>
#include <QTreeView>
#include <QHash>

//_______________________________________________________________
class ColumnSelectionMenu:public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    ColumnSelectionMenu( QWidget*, QTreeView*, const QString& = tr( "Select Columns" ) );

    private Q_SLOTS:

    //! update actions
    void _updateActions( void );

    //! update mask when triggering actions
    void _updateSelectedColumns( QAction* action );

    private:

    //! target
    QTreeView* target_;

    //! map action to column index
    using ActionMap = QHash< QAction*, int >;

    //! map action to column index
    ActionMap actions_;

};

#endif

#ifndef _ColumnSelectionMenu_h_
#define _ColumnSelectionMenu_h_

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"
#include "Debug.h"

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <QtCore/QHash>
#include <cassert>

//_______________________________________________________________
class ColumnSelectionMenu:public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    ColumnSelectionMenu( QWidget*, QTreeView*, const QString& title = "Select Columns" );

    private slots:

    //! update actions
    void _updateActions( void );

    //! update mask when triggering actions
    void _updateSelectedColumns( QAction* action );

    private:

    //! target
    QTreeView* target_;

    //! map action to column index
    typedef QHash< QAction*, int > ActionMap;

    //! map action to column index
    ActionMap actions_;

};

#endif

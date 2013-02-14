#ifndef _ContextMenu_h_
#define _ContextMenu_h_

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

#include "BaseContextMenu.h"

#include <QAction>
#include <QList>
#include <QPointer>

// implements context menu, installable on widgets
class ContextMenu: public BaseContextMenu
{

    Q_OBJECT

    public:

    //! contructor
    ContextMenu( QWidget* );

    //!destructor
    virtual ~ContextMenu( void )
    {}

    protected slots:

    //! add actions
    void _hideActions( void );

    //! show actions
    void _showActions( void );

    //! raise request
    void _raise( const QPoint& );

    private:

    typedef QPointer<QAction> ActionPointer;
    typedef QList<ActionPointer> ActionList;
    ActionList hiddenActions_;

};

#endif

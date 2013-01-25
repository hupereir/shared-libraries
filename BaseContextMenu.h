#ifndef _BaseContextMenu_h_
#define _BaseContextMenu_h_

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

#include <QtGui/QMenu>

// implements context menu, installable on widgets
class BaseContextMenu: public QMenu, public Counter
{

    public:

    //! contructor
    BaseContextMenu( QWidget* );

    //!destructor
    virtual ~BaseContextMenu( void )
    {}

    //!@name modifiers
    //@{

    //! ignore disabled action
    void setIgnoreDisabledActions( bool value )
    { ignoreDisabledActions_ = value; }

    //! clear
    void clear( void )
    {
        QMenu::clear();
        needSeparator_ = false;
    }

    //! set need separator
    /*! separator is added next time an action is added */
    void setNeedSeparator( bool value )
    { needSeparator_ = value; }

    //! add separator
    QAction* addSeparator( void );

    //! add menu
    QAction* addMenu( QMenu* );

    //! add action
    void addAction( QAction* );

    //@}

    private:

    //! ignore disabled actions
    bool ignoreDisabledActions_;

    //! need separator
    bool needSeparator_;

};

#endif

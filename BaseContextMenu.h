#ifndef BaseContextMenu_h
#define BaseContextMenu_h

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

#include <QPointer>
#include <QMenu>

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
    void setHideDisabledActions( bool value )
    { hideDisabledActions_ = value; }

    //! clear
    void clear( void )
    {
        QMenu::clear();
        needSeparator_ = false;
    }

    //! add separator
    QAction* addSeparator( void );

    //! add menu
    QAction* addMenu( QMenu* );

    //! add action
    void addAction( QAction* );

    //! add action
    void insertAction( QAction*, QAction* );

    //@}

    protected:

    bool _hideDisabledActions( void ) const
    { return hideDisabledActions_; }

    private:

    //! separator
    QPointer<QAction> separator_;

    //! ignore disabled actions
    bool hideDisabledActions_;

    //! need separator
    bool needSeparator_;

};

#endif

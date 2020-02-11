#ifndef BaseContextMenu_h
#define BaseContextMenu_h

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

#include "base_qt_export.h"
#include "Counter.h"

#include <QMenu>
#include <QPointer>

// implements context menu, installable on widgets
class BASE_QT_EXPORT BaseContextMenu: public QMenu, private Base::Counter<BaseContextMenu>
{

    Q_OBJECT

    public:

    //* contructor
    explicit BaseContextMenu( QWidget* );

    //*@name accessors
    //@{

    //* return true if the menu has no visible action
    bool isEmpty() const
    { return isEmpty_; }

    //@}

    //*@name modifiers
    //@{

    //* ignore disabled action
    void setHideDisabledActions( bool value )
    { hideDisabledActions_ = value; }

    //* clear
    void clear()
    {
        QMenu::clear();
        needSeparator_ = false;
    }

    //* add separator
    QAction* addSeparator();

    //* add menu
    QAction* addMenu( QMenu* );

    //* add action
    void addAction( QAction* );

    //* add action
    void insertAction( QAction*, QAction* );

    //@}

    protected:

    bool _hideDisabledActions() const
    { return hideDisabledActions_; }

    private:

    //* separator
    QPointer<QAction> separator_;

    //* ignore disabled actions
    bool hideDisabledActions_ = false;

    //* need separator
    bool needSeparator_ = false;

    //* true if empty
    bool isEmpty_ = true;

};

#endif

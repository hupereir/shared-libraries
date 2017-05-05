#ifndef CustomMenu_h
#define CustomMenu_h

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
#include "WeakPointer.h"

#include <QAction>
#include <QKeyEvent>
#include <QMenu>
#include <QList>


//* some basic runtime debuging
class CustomMenu: public QMenu, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    CustomMenu( QWidget* = nullptr );

    //* constructor
    CustomMenu( const QString&, QWidget* = nullptr );

    //*@name modifiers
    //@{

    //* add actions that become visible only when pressing the shift key
    void addInvisibleAction( QAction* );

    //* clear
    void clear( void );

    //@}

    protected:

    //* key press
    void keyPressEvent( QKeyEvent* ) override;

    //* key release
    void keyReleaseEvent( QKeyEvent* ) override;

    private:

    //* action list
    using ActionPointer=Base::WeakPointer<QAction>;
    using ActionList=QList<ActionPointer>;
    ActionList invisibleActions_;

};

#endif

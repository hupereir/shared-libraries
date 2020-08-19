#ifndef ReverseOrderAction_h
#define ReverseOrderAction_h

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
#include "Debug.h"

#include <QAction>
#include <QTreeView>
#include <QHeaderView>

//_______________________________________________________________
class BASE_QT_EXPORT ReverseOrderAction:public QAction, private Base::Counter<ReverseOrderAction>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit ReverseOrderAction( QWidget*, QTreeView*, const QString& = tr( "Reverse Sort Order" ) );

    //* constructor
    explicit ReverseOrderAction( QWidget*, QHeaderView*, const QString&  = tr( "Reverse Sort Order" ) );

    private:

    //* private constructor
    explicit ReverseOrderAction( QWidget*, const QString& );

    //* revert order
    void _reverseOrder();

    //* target
    QTreeView* target_ = nullptr;

    //* header
    QHeaderView* header_ = nullptr;

};

#endif

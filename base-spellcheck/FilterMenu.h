#ifndef FilterMenu_h
#define FilterMenu_h

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


#include "base_spellcheck_export.h"
#include "Counter.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QHash>

namespace SpellCheck
{

    //* dictionary filter selection menu
    class BASE_SPELLCHECK_EXPORT FilterMenu: public QMenu, private Base::Counter<FilterMenu>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit FilterMenu( QWidget* );

        //* select filter manualy
        void select( const QString& );

        //* reset filter list
        void reset();

        Q_SIGNALS:

        //* filter changed
        void selectionChanged( const QString& );

        private:

        //* filter selected from menu
        void _selectFilter( QAction* );

        //* action group
        QActionGroup* group_ = nullptr;

        //* map filters and action
        using ActionMap = QHash<QAction*, QString>;
        ActionMap actions_;

    };

}

#endif

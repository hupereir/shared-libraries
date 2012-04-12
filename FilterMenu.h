#ifndef FilterMenu_h
#define FilterMenu_h

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

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QMenu>
#include <QtCore/QHash>

namespace SPELLCHECK
{

    //! dictionary filter selection menu
    class FilterMenu: public QMenu, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        FilterMenu( QWidget* parent );

        //! select filter manualy
        void select( const QString& filter );

        signals:

        //! filter changed
        void selectionChanged( const QString& );

        public slots:

        //! reset filter list
        void reset( void );

        protected slots:

        //! filter selected from menu
        void _selectFilter( QAction* action );

        private:

        //! action group
        QActionGroup* group_;

        //! map filters and action
        typedef QHash<QAction*, QString> ActionMap;
        ActionMap actionMap_;

    };

};

#endif

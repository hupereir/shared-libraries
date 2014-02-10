#ifndef DictionaryMenu_h
#define DictionaryMenu_h

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

/*!
\file DictionaryMenu.h
\brief dictionary dictionary selection menu
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Counter.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QHash>

//! dictionary dictionary selection menu
namespace SpellCheck
{
    class DictionaryMenu: public QMenu, public Counter
    {

        Q_OBJECT

        public:

        //! constructor
        DictionaryMenu( QWidget* );

        //! select dictionary manualy
        void select( const QString& );

        Q_SIGNALS:

        //! dictionary changed
        void selectionChanged( const QString& );

        public Q_SLOTS:

        //! reset dictionary list
        void reset( void );

        protected Q_SLOTS:

        //! dictionary selected from menu
        void _selectDictionary( QAction* action );

        private:

        //! action group
        QActionGroup* group_;

        //! map dictionaries and action
        typedef QHash<QAction*, QString> ActionMap;
        ActionMap actionMap_;

    };

};

#endif

#ifndef HelpManager_h
#define HelpManager_h

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
#include "Debug.h"
#include "File.h"
#include "HelpItem.h"

#include <QObject>
#include <QAction>

//* help manager class
namespace Base
{

    class HelpManager:public QObject, private Counter<HelpManager>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit HelpManager( QObject* parent );

        //* destructor
        virtual ~HelpManager( void )
        { Debug::Throw( "HelpManager::~HelpManager.\n" ); }

        //*@name accessors
        //@{

        //* install help. Put items in the menu, create associated objects
        void install( const HelpItem::List& items )
        { items_ = items; }

        //* install help. Put items in the menu, create associated objects
        void install( const QString text[], bool clear = true );

        //* list of items
        const HelpItem::List& items( void )
        { return items_; }

        //* clear help
        void clear( void )
        { items_.clear(); }

        //* set dialog caption
        void setWindowTitle( const QString& value );

        //@}

        //* display action
        QAction& displayAction( void )
        { return *displayAction_; }

        protected Q_SLOTS:

        //* opens help dialog
        void _display( void );

        private:

        //* dialog display action
        QAction* displayAction_ = nullptr;

        //* window title
        QString windowTitle_;

        //* complete set of help items
        HelpItem::List items_;

    };

};

#endif

#ifndef SuggestionMenu_h
#define SuggestionMenu_h

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
#include "SpellInterface.h"

#include <QMenu>
#include <QHash>

namespace SpellCheck {

    //* customized popup menu to display spell checker suggestions
    class SuggestionMenu: public QMenu, public Counter
    {

        //* Qt meta object declaration
        Q_OBJECT

        public:

        //* constructor
        SuggestionMenu( QWidget *parent, const QString&, bool = false );

        //* destructor
        virtual ~SuggestionMenu() = default;

        //*@name accessors
        //@{

        //* spell interface
        SpellInterface& interface( void )
        { return interface_; }

        //@}

        Q_SIGNALS:

        //* signal emited when a file is selected
        void suggestionSelected( QString );

        //* ignore word
        void ignoreWord( QString );

        private Q_SLOTS:

        //* load suggestions before showing the menu
        void _aboutToShow( void );

        //* select word from action
        void _select( QAction* );

        //* add word to dictionary
        void _addWord( void );

        //* ignore word
        void _ignoreWord( void )
        {
            if( word_.isEmpty() ) return;
            emit ignoreWord( word_ );
        }

        private:

        //* spell interface
        SpellInterface interface_;

        //* misspelled word
        QString word_;

        //* map actions to suggested words
        using SuggestionMap = QHash<QAction*, QString>;
        SuggestionMap suggestions_;

    };
}

#endif

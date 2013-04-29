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

#include "DictionaryMenu.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"

namespace SPELLCHECK
{
    //____________________________________________________________________
    DictionaryMenu::DictionaryMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( "DictionaryMenu" )
    {

        Debug::Throw( "DictionaryMenu::DictionaryMenu.\n" );
        setTitle( tr( "Dictionary" ) );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        // reset
        reset();

        // connections
        connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selectDictionary( QAction* ) ) );

    }

    //____________________________________________________________________
    void DictionaryMenu::select( const QString& dictionary )
    {
        Debug::Throw( "DictionaryMenu::select.\n" );

        for( ActionMap::iterator iter = actionMap_.begin(); iter != actionMap_.end(); ++iter )
        { if( iter.value() == dictionary ) iter.key()->setChecked( true ); }

        return;

    }

    //____________________________________________________________________
    void DictionaryMenu::reset( void )
    {

        Debug::Throw( "DictionaryMenu::reset.\n" );

        // store selected dictionary
        QString selection;
        for( ActionMap::iterator iter = actionMap_.begin(); iter != actionMap_.end(); ++iter )
        { if( iter.key()->isChecked() ) selection = iter.value(); }

        // clear actions
        QMenu::clear();
        actionMap_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( ICONS::RELOAD ), tr( "Reload" ), this ) );
        connect( action, SIGNAL( triggered() ), SLOT( reset() ) );

        // load disabled dictionaries from options
        QStringList disabledDictionaries;
        if( XmlOptions::get().contains( "SPELLCHECK_DISABLED_DICTIONARIES" ) )
        { disabledDictionaries = QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_DICTIONARIES" ) ).split( " " ); }

        // populate list
        bool first( true );
        foreach( const QString& dictionary, SPELLCHECK::SpellInterface().dictionaries() )
        {

            // check against list of disabled dictionaries
            if( disabledDictionaries.contains( dictionary ) ) continue;

            // insert separator
            if( first )
            {
                first = false;
                addSeparator();
            }

            // insert action
            QAction* action( new QAction( dictionary, this ) );
            action->setCheckable( true );
            action->setChecked( dictionary == selection );
            actionMap_.insert( action, dictionary );
            addAction( action );
            group_->addAction( action );

        }

    }

    //______________________________________________________________________________________
    void DictionaryMenu::_selectDictionary( QAction*  action )
    {

        Debug::Throw( "DictionaryMenu::_dictionary.\n" );
        ActionMap::iterator iter( actionMap_.find( action ) );
        if( iter == actionMap_.end() ) return;

        select( iter.value() );
        emit selectionChanged( iter.value() );
        return;

    }
}

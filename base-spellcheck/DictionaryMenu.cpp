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

#include "CppUtil.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"

namespace SpellCheck
{
    //____________________________________________________________________
    DictionaryMenu::DictionaryMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( QStringLiteral("DictionaryMenu") )
    {

        Debug::Throw( QStringLiteral("DictionaryMenu::DictionaryMenu.\n") );
        setTitle( tr( "Dictionary" ) );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        // reset
        reset();

        // connections
        connect( this, &QMenu::triggered, this, &DictionaryMenu::_selectDictionary );

    }

    //____________________________________________________________________
    void DictionaryMenu::select( const QString& dictionary )
    {
        Debug::Throw() << "DictionaryMenu::select - dictionary: " << dictionary << endl;
        const auto iter = Base::findByValue( actions_, dictionary );
        if( iter != actions_.end() ) iter.key()->setChecked( true );
        return;
    }

    //____________________________________________________________________
    void DictionaryMenu::reset()
    {

        Debug::Throw( QStringLiteral("DictionaryMenu::reset.\n") );

        // store selected dictionary
        QString selection;
        auto iter = std::find_if( actions_.keyBegin(), actions_.keyEnd(), [](QAction* current){ return current->isChecked(); } );
        if( iter != actions_.keyEnd() ) selection = iter.base().value();

        // clear actions
        QMenu::clear();
        actions_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload" ), this ) );
        connect( action, &QAction::triggered, this, &DictionaryMenu::reset );

        // load disabled dictionaries from options
        QStringList disabledDictionaries;
        if( XmlOptions::get().contains( QStringLiteral("SPELLCHECK_DISABLED_DICTIONARIES") ) )
        { disabledDictionaries = QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_DICTIONARIES") ) ).split( QLatin1Char(' ') ); }

        // populate list
        bool first( true );
        auto dictionaries( SpellCheck::SpellInterface().dictionaries() );
        for( const auto& dictionary:dictionaries )
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
            auto action( addAction( dictionary ) );
            action->setCheckable( true );
            action->setChecked( dictionary == selection );
            actions_.insert( action, dictionary );
            addAction( action );
            group_->addAction( action );

        }

    }

    //______________________________________________________________________________________
    void DictionaryMenu::_selectDictionary( QAction*  action )
    {

        Debug::Throw( QStringLiteral("DictionaryMenu::_selectDictionary.\n") );
        const auto iter( actions_.find( action ) );
        if( iter == actions_.end() ) return;

        select( iter.value() );
        emit selectionChanged( iter.value() );
        return;

    }
}

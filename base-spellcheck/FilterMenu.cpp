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

#include "FilterMenu.h"
#include "BaseIconNames.h"
#include "CppUtil.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"


namespace SpellCheck
{

    //____________________________________________________________________
    FilterMenu::FilterMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( QStringLiteral("FilterMenu") )
    {
        Debug::Throw( QStringLiteral("FilterMenu::FilterMenu.\n") );
        setTitle( tr( "Filter" ) );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        reset();
        connect( this, &QMenu::triggered, this, &FilterMenu::_selectFilter );
    }

    //____________________________________________________________________
    void FilterMenu::select( const QString& filter )
    {
        Debug::Throw() << "FilterMenu::select - filter: " << filter << Qt::endl;
        const auto iter = Base::findByValue( actions_, filter );
        if( iter != actions_.cend() ) iter.key()->setChecked( true );
    }

    //____________________________________________________________________
    void FilterMenu::reset()
    {
        Debug::Throw( QStringLiteral("FilterMenu::reset.\n") );

        // store selected filter
        QString selection;
        auto iter = std::find_if( actions_.keyBegin(), actions_.keyEnd(), [](QAction* current){ return current->isChecked(); } );
        if( iter != actions_.keyEnd() ) selection = iter.base().value();

        // clear actions
        QMenu::clear();
        actions_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload" ), this ) );
        connect( action, &QAction::triggered, this, &FilterMenu::reset );

        // load disabled filters from options
        QStringList disabledFilters;
        if( XmlOptions::get().contains( QStringLiteral("SPELLCHECK_DISABLED_FILTERS") ) )
        { disabledFilters = QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_FILTERS") ) ).split( QLatin1Char(' ') ); }

        // populate list
        bool first( true );
        for( const auto& filter:SpellCheck::SpellInterface().filters() )
        {

            // check against list of disabled filters
            if( disabledFilters.contains( filter ) ) continue;

            // insert separator
            if( first )
            {
                first = false;
                addSeparator();
            }

            // insert action
            auto action( addAction( filter ) );
            action->setCheckable( true );
            action->setChecked( filter == selection );
            actions_.insert( action, filter );
            group_->addAction( action );
        }
    }

    //______________________________________________________________________________________
    void FilterMenu::_selectFilter( QAction*  action )
    {
        Debug::Throw( QStringLiteral("FilterMenu::_filter.\n") );
        const auto iter( actions_.find( action ) );
        if( iter == actions_.cend() ) return;

        select( iter.value() );
        emit selectionChanged( iter.value() );
    }
}

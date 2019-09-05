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

#include "CppUtil.h"
#include "BaseIconNames.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"

namespace SpellCheck
{

    //____________________________________________________________________
    FilterMenu::FilterMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( "FilterMenu" )
    {
        Debug::Throw( "FilterMenu::FilterMenu.\n" );
        setTitle( tr( "Filter" ) );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        reset();
        connect( this, SIGNAL(triggered(QAction*)), SLOT(_selectFilter(QAction*)) );
    }

    //____________________________________________________________________
    void FilterMenu::select( const QString& filter )
    {

        Debug::Throw() << "FilterMenu::select - filter: " << filter << endl;
        const auto iter = Base::findByValue( actions_, filter );
        if( iter != actions_.end() ) iter.key()->setChecked( true );
        return;

    }

    //____________________________________________________________________
    void FilterMenu::reset()
    {

        Debug::Throw( "FilterMenu::reset.\n" );

        // store selected filter
        QString selection;
        #if QT_VERSION >= 0x050600
        auto iter = std::find_if( actions_.keyBegin(), actions_.keyEnd(), [](QAction* current){ return current->isChecked(); } );
        if( iter != actions_.keyEnd() ) selection = iter.base().value();
        #else
        for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
        { if( iter.key()->isChecked() ) selection = iter.value(); }
        #endif

        // clear actions
        QMenu::clear();
        actions_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( IconNames::Reload ), tr( "Reload" ), this ) );
        connect( action, SIGNAL(triggered()), SLOT(reset()) );

        // load disabled filters from options
        QStringList disabledFilters;
        if( XmlOptions::get().contains( "SPELLCHECK_DISABLED_FILTERS" ) )
        { disabledFilters = QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_FILTERS" ) ).split( QLatin1Char(' ') ); }

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

        Debug::Throw( "FilterMenu::_filter.\n" );
        const auto iter( actions_.find( action ) );
        if( iter == actions_.end() ) return;

        select( iter.value() );
        emit selectionChanged( iter.value() );
        return;

    }
}

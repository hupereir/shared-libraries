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

#include "FilterMenu.h"
#include "FilterMenu.moc"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "SpellInterface.h"
#include "XmlOptions.h"

namespace SPELLCHECK
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

        for( ActionMap::iterator iter = actionMap_.begin(); iter != actionMap_.end(); ++iter )
        { if( iter.value() == filter ) iter.key()->setChecked( true ); }

        return;

    }

    //____________________________________________________________________
    void FilterMenu::reset( void )
    {

        Debug::Throw( "FilterMenu::reset.\n" );

        // store selected filter
        QString selection;
        for( ActionMap::iterator iter = actionMap_.begin(); iter != actionMap_.end(); ++iter )
        { if( iter.key()->isChecked() ) selection = iter.value(); }

        // clear actions
        QMenu::clear();
        actionMap_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( ICONS::RELOAD ), tr( "Reload" ), this ) );
        connect( action, SIGNAL(triggered()), SLOT(reset()) );

        // load disabled filters from options
        QStringList disabledFilters;
        if( XmlOptions::get().contains( "SPELLCHECK_DISABLED_FILTERS" ) )
        { disabledFilters = QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_FILTERS" ) ).split( " " ); }

        // populate list
        bool first( true );
        foreach( const QString& filter, SPELLCHECK::SpellInterface().filters() )
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
            QAction* action( new QAction( filter, this ) );
            action->setCheckable( true );
            action->setChecked( filter == selection );
            actionMap_.insert( action, filter );
            addAction( action );
            group_->addAction( action );
        }

    }

    //______________________________________________________________________________________
    void FilterMenu::_selectFilter( QAction*  action )
    {

        Debug::Throw( "FilterMenu::_filter.\n" );
        ActionMap::iterator iter( actionMap_.find( action ) );
        if( iter == actionMap_.end() ) return;

        select( iter.value() );
        emit selectionChanged( iter.value() );
        return;

    }
}

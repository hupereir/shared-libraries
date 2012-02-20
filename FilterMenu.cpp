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

/*!
   \file FilterMenu.cpp
   \brief dictionary filter selection menu
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "BaseIcons.h"
#include "FilterMenu.h"
#include "IconEngine.h"
#include "SpellInterface.h"

namespace SPELLCHECK
{

    //____________________________________________________________________
    FilterMenu::FilterMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( "FilterMenu" )
    {
        Debug::Throw( "FilterMenu::FilterMenu.\n" );
        setTitle( "&Filter" );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        _reset();
        connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selectFilter( QAction* ) ) );
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
    void FilterMenu::_reset( void )
    {

        Debug::Throw( "FilterMenu::_reset.\n" );

        // clear actions
        QMenu::clear();
        actionMap_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
        connect( action, SIGNAL( triggered() ), SLOT( _reset() ) );

        // load filters from spell interface
        QSet< QString > filters( SPELLCHECK::SpellInterface().filters() );
        if( !filters.empty() ) addSeparator();
        for( QSet<QString>::iterator iter = filters.begin(); iter != filters.end(); ++iter )
        {
            QAction* action( new QAction( *iter, this ) );
            action->setCheckable( true );
            actionMap_.insert( action, *iter );
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

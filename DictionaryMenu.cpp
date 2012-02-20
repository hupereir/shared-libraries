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
   \file DictionaryMenu.cpp
   \brief dictionary dictionary selection menu
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "BaseIcons.h"
#include "DictionaryMenu.h"
#include "IconEngine.h"
#include "SpellInterface.h"

namespace SPELLCHECK
{
    //____________________________________________________________________
    DictionaryMenu::DictionaryMenu( QWidget* parent ):
        QMenu( parent ),
        Counter( "DictionaryMenu" )
    {

        Debug::Throw( "DictionaryMenu::DictionaryMenu.\n" );
        setTitle( "&Dictionary" );

        // action group
        group_ = new QActionGroup( this );
        group_->setExclusive( true );

        _reset();
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
    void DictionaryMenu::_reset( void )
    {

        Debug::Throw( "DictionaryMenu::_reset.\n" );

        // store selected dictionary
        QString dictionary;
        for( ActionMap::iterator iter = actionMap_.begin(); iter != actionMap_.end(); ++iter )
        { if( iter.key()->isChecked() ) dictionary = iter.value(); }

        // clear actions
        QMenu::clear();
        actionMap_.clear();

        // add reset button
        QAction* action;
        addAction( action = new QAction( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
        connect( action, SIGNAL( triggered() ), SLOT( _reset() ) );

        // load dictionaries from spell interface
        QSet< QString > dictionaries( SPELLCHECK::SpellInterface().dictionaries() );
        if( !dictionaries.empty() ) addSeparator();

        for( QSet<QString>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); ++iter )
        {
            QAction* action( new QAction( *iter, this ) );
            action->setCheckable( true );
            action->setChecked( *iter == dictionary );
            actionMap_.insert( action, *iter );
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

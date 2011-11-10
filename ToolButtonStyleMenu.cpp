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
\file ToolButtonStyleMenu.cpp
\brief provides tool button style selection menu
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Debug.h"
#include "ToolButtonStyleMenu.h"

#include <cassert>

//_____________________________________________________________________________
ToolButtonStyleMenu::ToolButtonStyleMenu( QWidget* parent ):
QMenu( "&Text position", parent ),
Counter( "ToolButtonStyleMenu" )
{
    Debug::Throw( "ToolButtonStyleMenu::ToolButtonStyleMenu.\n" );

    QActionGroup *group = new QActionGroup( this );
    connect( group, SIGNAL( triggered( QAction* ) ), SLOT( _selected( QAction* ) ) );

    // install values
    typedef std::list< std::pair<QString, Qt::ToolButtonStyle > > List;
    static List actionNames;
    if( actionNames.empty() )
    {
        actionNames.push_back( std::make_pair( "&Icons Only", Qt::ToolButtonIconOnly ) );
        actionNames.push_back( std::make_pair( "&Text Only", Qt::ToolButtonTextOnly ) );
        actionNames.push_back( std::make_pair( "Text &Alongside icons", Qt::ToolButtonTextBesideIcon ) );
        actionNames.push_back( std::make_pair( "Text &Under icons", Qt::ToolButtonTextUnderIcon ) );
    }

    // generic action
    for( List::const_iterator iter = actionNames.begin(); iter != actionNames.end(); ++iter )
    {
        QAction* action = new QAction( iter->first, this );
        addAction( action );
        action->setCheckable( true );
        actions_.insert( std::make_pair( action, iter->second ) );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void ToolButtonStyleMenu::select( Qt::ToolButtonStyle size )
{

    Debug::Throw( "ToolButtonStyleMenu::select.\n" );
    for( ActionMap::const_iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        if( iter->second == size )
        {
            iter->first->setChecked( true );
            return;
        }
    }

    assert(0);
}

//_____________________________________________________________________________
void ToolButtonStyleMenu::_selected( QAction* action )
{

    Debug::Throw( "ToolButtonStyleMenu::_selected.\n" );

    // find matching actions
    ActionMap::const_iterator iter = actions_.find( action );
    assert( iter != actions_.end() );
    emit styleSelected( iter->second );

}

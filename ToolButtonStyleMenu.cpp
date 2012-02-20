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
    typedef QList< QPair<QString, Qt::ToolButtonStyle > > List;
    List actionNames;
    actionNames
        << QPair<QString, Qt::ToolButtonStyle >( "&Icons Only", Qt::ToolButtonIconOnly )
        << QPair<QString, Qt::ToolButtonStyle >( "&Text Only", Qt::ToolButtonTextOnly )
        << QPair<QString, Qt::ToolButtonStyle >( "Text &Alongside icons", Qt::ToolButtonTextBesideIcon )
        << QPair<QString, Qt::ToolButtonStyle >( "Text &Under icons", Qt::ToolButtonTextUnderIcon );

    // generic action
    for( List::const_iterator iter = actionNames.begin(); iter != actionNames.end(); ++iter )
    {
        QAction* action = new QAction( iter->first, this );
        addAction( action );
        action->setCheckable( true );
        actions_.insert( action, iter->second );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void ToolButtonStyleMenu::select( Qt::ToolButtonStyle size )
{

    Debug::Throw( "ToolButtonStyleMenu::select.\n" );
    for( ActionMap::const_iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        if( iter.value() == size )
        {
            iter.key()->setChecked( true );
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
    emit styleSelected( iter.value() );

}

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
\file IconSizeMenu.cpp
\brief provides icon size selection menu
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Debug.h"
#include "IconSizeMenu.h"

#include <cassert>

//_____________________________________________________________________________
IconSizeMenu::IconSizeMenu( QWidget* parent ):
QMenu( "&Icon size", parent ),
Counter( "IconSizeMenu" )
{
    Debug::Throw( "IconSizeMenu::IconSizeMenu.\n" );

    QActionGroup *group = new QActionGroup( this );
    connect( group, SIGNAL( triggered( QAction* ) ), SLOT( _selected( QAction* ) ) );

    const IconSize::Map& sizes( IconSize::map() );

    // generic action
    QAction* action;
    for( IconSize::Map::const_iterator iter = sizes.begin(); iter != sizes.end(); ++iter )
    {
        addAction( action = new QAction( iter.value(), this ) );
        action->setCheckable( true );
        actions_.insert( action, iter.key() );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void IconSizeMenu::select( IconSize::Size size )
{

    Debug::Throw( "IconSizeMenu::select.\n" );
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
void IconSizeMenu::_selected( QAction* action )
{

    Debug::Throw( "IconSizeMenu::_selected.\n" );

    // find matching actions
    ActionMap::const_iterator iter = actions_.find( action );
    assert( iter != actions_.end() );
    emit iconSizeSelected( iter.value() );

}

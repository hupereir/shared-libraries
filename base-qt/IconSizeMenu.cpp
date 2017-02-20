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

#include "Debug.h"
#include "IconSizeMenu.h"

//_____________________________________________________________________________
IconSizeMenu::IconSizeMenu( QWidget* parent, bool custom ):
    QMenu( tr( "Icon size" ), parent ),
    Counter( "IconSizeMenu" )
{
    Debug::Throw( "IconSizeMenu::IconSizeMenu.\n" );

    QActionGroup *group = new QActionGroup( this );
    connect( group, SIGNAL(triggered(QAction*)), SLOT(_selected(QAction*)) );

    const IconSize::Map& sizes( IconSize::map() );

    // generic action
    QAction* action;
    for( auto iter = sizes.begin(); iter != sizes.end(); ++iter )
    {

        // skip some items depending on custom flags
        if( custom && iter.key() == IconSize::Default ) continue;
        if( !custom && ( iter.key() == IconSize::Maximum || iter.key() == IconSize::Oversized ) ) continue;

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
    for( auto iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        if( iter.value() == size )
        {
            iter.key()->setChecked( true );
            return;
        }
    }

}

//_____________________________________________________________________________
void IconSizeMenu::_selected( QAction* action )
{

    Debug::Throw( "IconSizeMenu::_selected.\n" );

    // find matching actions
    ActionMap::const_iterator iter = actions_.find( action );
    Q_ASSERT( iter != actions_.end() );
    emit iconSizeSelected( iter.value() );

}

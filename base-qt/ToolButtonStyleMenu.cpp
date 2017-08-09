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

#include "ToolButtonStyleMenu.h"

#include "CppUtil.h"
#include "Debug.h"

#include <QActionGroup>

//_____________________________________________________________________________
ToolButtonStyleMenu::ToolButtonStyleMenu( QWidget* parent ):
    QMenu( tr( "Text position" ), parent ),
    Counter( "ToolButtonStyleMenu" )
{
    Debug::Throw( "ToolButtonStyleMenu::ToolButtonStyleMenu.\n" );

    auto group = new QActionGroup( this );
    connect( group, SIGNAL(triggered(QAction*)), SLOT(_selected(QAction*)) );

    // install values
    using NamePair=QPair<QString, int >;
    using List=QList< NamePair >;
    auto actionNames = Base::makeT<List>( {
        { tr( "System Default" ), -1 },
        { tr( "No Text" ), Qt::ToolButtonIconOnly },
        { tr( "Text Only" ), Qt::ToolButtonTextOnly },
        { tr( "Text Alongside icons" ), Qt::ToolButtonTextBesideIcon },
        { tr( "Text Under icons" ), Qt::ToolButtonTextUnderIcon } } );

    // generic action
    for( const auto& namePair:actionNames )
    {
        auto action = new QAction( namePair.first, this );
        addAction( action );
        action->setCheckable( true );
        actions_.insert( action, namePair.second );
        group->addAction( action );
    }

}

//_____________________________________________________________________________
void ToolButtonStyleMenu::select( int style )
{

    Debug::Throw() << "ToolButtonStyleMenu::select - style: " << style << endl;
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        if( iter.value() == style )
        {
            iter.key()->setChecked( true );
            return;
        }
    }

}

//_____________________________________________________________________________
void ToolButtonStyleMenu::_selected( QAction* action )
{

    Debug::Throw( "ToolButtonStyleMenu::_selected.\n" );

    // find matching actions
    auto iter = actions_.find( action );
    Q_ASSERT( iter != actions_.end() );
    emit styleSelected( iter.value() );

}

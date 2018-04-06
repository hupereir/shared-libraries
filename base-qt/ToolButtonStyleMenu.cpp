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
    actions_ = Base::makeT<ActionMap>(
    {
        { addAction( tr( "System Default" ) ), Qt::ToolButtonFollowStyle },
        { addAction( tr( "No Text" ) ), Qt::ToolButtonIconOnly },
        { addAction( tr( "Text Only" ) ), Qt::ToolButtonTextOnly },
        { addAction( tr( "Text Alongside icons" ) ), Qt::ToolButtonTextBesideIcon },
        { addAction( tr( "Text Under icons" ) ), Qt::ToolButtonTextUnderIcon }
    } );

    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    {
        iter.key()->setCheckable( true );
        group->addAction( iter.key() );
    }

}

//_____________________________________________________________________________
void ToolButtonStyleMenu::select( int style )
{

    Debug::Throw() << "ToolButtonStyleMenu::select - style: " << style << endl;

    // adjust style
    if( style < 0 ) style = Qt::ToolButtonFollowStyle;

    // find in actions
    const auto iter = Base::findByValue( actions_, style );

    // assign
    if( iter != actions_.end() ) iter.key()->setChecked( true );

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

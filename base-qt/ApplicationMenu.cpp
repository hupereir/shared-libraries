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

#include "ApplicationMenu.h"
#include "BaseIconNames.h"
#include "IconEngine.h"

//_____________________________________________________
ApplicationMenu::ApplicationMenu( QWidget* parent, QWidget* target ):
    QMenu( parent ),
    Counter( "ApplicationMenu" ),
    target_( target )
{
    Debug::Throw( QStringLiteral( "ApplicationMenu::ApplicationMenu.\n" ) );
    connect( this, &ApplicationMenu::aboutToShow, this, &ApplicationMenu::updateMenu );
    updateMenu();
}

//_____________________________________________________
void ApplicationMenu::setTarget( QWidget* target )
{
    target_ = target;
    updateMenu();
}

//_____________________________________________________
void ApplicationMenu::updateMenu()
{
    Debug::Throw( QStringLiteral("ApplicationMenu::updateMenu.\n" ) );

    // clear
    clear();

    // check target
    if( !target_ ) return;

    // loop over QMenu children
    bool needSeparator = false;
    for( const auto& action:target_->actions() )
    {
        if( auto menu = action->menu() )
        {
            updateFrom( menu, needSeparator );
            needSeparator = !menu->isEmpty();
        }
    }
}

//_____________________________________________________
void ApplicationMenu::updateFrom( QMenu* menu, bool needSeparator )
{

    Debug::Throw( QStringLiteral("ApplicationMenu::updateFrom.\n") );

    // trigger menu update
    emit menu->aboutToShow();

    // loop over actions
    bool first = true;
    for( const auto& action:menu->actions() )
    {
        if( !action->isVisible() ) continue;
        if( first && needSeparator ) addSeparator();
        addAction( action );
        first = false;
    }
    return;
}

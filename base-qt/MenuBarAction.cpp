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

#include "MenuBarAction.h"

#include "BaseIconNames.h"
#include "IconEngine.h"

#include <QToolButton>

//_____________________________________________________
MenuBarAction::MenuBarAction( QObject* parent, QWidget* target ):
    QWidgetAction( parent ),
    Counter( "MenuBarAction" ),
    target_( target ),
    menu_( new QMenu )
{
    Debug::Throw( QStringLiteral( "MenuBarAction::MenuBarAction.\n" ) );

    auto toolButton = new QToolButton;
    toolButton->setText( tr( "Show menu" ) );
    toolButton->setIcon( IconEngine::get( IconNames::Menu ) );
    toolButton->setMenu( menu_ );
    toolButton->setPopupMode(QToolButton::InstantPopup);
    setDefaultWidget(toolButton);

    connect( menu_, &QMenu::aboutToShow, this, &MenuBarAction::updateMenu );
}

//_____________________________________________________
void MenuBarAction::updateMenu()
{

    Debug::Throw( QStringLiteral("MenuBarAction::updateMenu.\n" ) );

    // clear
    menu_->clear();

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
void MenuBarAction::updateFrom( QMenu* menu, bool needSeparator )
{

    Debug::Throw( QStringLiteral("MenuBarAction::updateFrom.\n") );

    // trigger menu update
    emit menu->aboutToShow();

    // loop over actions
    bool first = true;
    for( const auto& action:menu->actions() )
    {
        if( auto menu = action->menu() ) updateFrom( menu, needSeparator || !first );
        else {
            if( first && needSeparator ) menu_->addSeparator();
            menu_->addAction( action );
        }
        first = false;
    }
    return;
}

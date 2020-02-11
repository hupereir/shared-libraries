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

#include "ContextMenu.h"
#include "Debug.h"

#include <QAbstractScrollArea>

//___________________________________________________
ContextMenu::ContextMenu( QWidget* parent ):
    BaseContextMenu( parent )
{

    Debug::Throw( QStringLiteral("ContextMenu::ContextMenu.\n") );
    parent->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( parent, &QWidget::customContextMenuRequested, this, &ContextMenu::_raise );
    connect( this, &QMenu::aboutToShow, this, &ContextMenu::_hideActions );
    connect( this, &QMenu::aboutToHide, this, &ContextMenu::_showActions );
}

//___________________________________________________
void ContextMenu::_raise( QPoint position )
{

    QAbstractScrollArea* view(  qobject_cast<QAbstractScrollArea*>( parentWidget() ) );
    if( view )  exec( view->viewport()->mapToGlobal( position ) );
    else exec( parentWidget()->mapToGlobal( position ) );

}

//___________________________________________________
void ContextMenu::_hideActions()
{

    Debug::Throw() << "ContextMenu::_hideActions" << endl;

    // hidden actions
    hiddenActions_.clear();
    if( _hideDisabledActions() )
    {

        for( const auto& action:actions() )
        {
            if( !action->isEnabled() && action->isVisible() )
            {
                Debug::Throw() << "ContextMenu::_hideActions - hiding: " << action->text() << endl;
                hiddenActions_.append( action );
                action->setVisible( false );
            }
        }
    }

}

//___________________________________________________
void ContextMenu::_showActions()
{
    Debug::Throw() << "ContextMenu::_showActions" << endl;
    if( _hideDisabledActions() )
    {

        for( const auto& action:hiddenActions_ )
        {
            if( action ) action.data()->setVisible( true );

        }

    }

    hiddenActions_.clear();

}

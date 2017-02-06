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

    Debug::Throw( "ContextMenu::ContextMenu.\n" );
    parent->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( parent, SIGNAL(customContextMenuRequested(QPoint)), SLOT(_raise(QPoint)) );
    connect( this, SIGNAL(aboutToShow()), SLOT(_hideActions()) );
    connect( this, SIGNAL(aboutToHide()), SLOT(_showActions()) );
}

//___________________________________________________
void ContextMenu::_raise( const QPoint& position )
{

    QAbstractScrollArea* view(  qobject_cast<QAbstractScrollArea*>( parentWidget() ) );
    if( view )  exec( view->viewport()->mapToGlobal( position ) );
    else exec( parentWidget()->mapToGlobal( position ) );

}

//___________________________________________________
void ContextMenu::_hideActions( void )
{

    Debug::Throw() << "ContextMenu::_hideActions" << endl;

    // hidden actions
    hiddenActions_.clear();
    if( _hideDisabledActions() )
    {

        for( auto action:actions() )
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
void ContextMenu::_showActions( void )
{
    Debug::Throw() << "ContextMenu::_showActions" << endl;
    if( _hideDisabledActions() )
    {

        for( auto action:hiddenActions_ )
        {
            if( action )
            {
                Debug::Throw() << "ContextMenu::_hideActions - showing: " << action.data()->text() << endl;
                action.data()->setVisible( true );
            }

        }

    }

    hiddenActions_.clear();

}

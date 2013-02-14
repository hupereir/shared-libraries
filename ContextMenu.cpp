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

#include "ContextMenu.h"
#include "Debug.h"

#include <QAbstractScrollArea>

//___________________________________________________
ContextMenu::ContextMenu( QWidget* parent ):
    BaseContextMenu( parent )
{

    Debug::Throw( "ContextMenu::ContextMenu.\n" );
    parent->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( parent, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raise( const QPoint& ) ) );
    connect( this, SIGNAL( aboutToShow( void ) ), SLOT( _hideActions( void ) ) );
    connect( this, SIGNAL( aboutToHide( void ) ), SLOT( _showActions( void ) ) );
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

        foreach( QAction* action, actions() )
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

        foreach( ActionPointer action, hiddenActions_ )
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

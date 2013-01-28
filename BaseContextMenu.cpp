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

#include "BaseContextMenu.h"
#include "Debug.h"

#include <QtGui/QAbstractScrollArea>

//___________________________________________________
BaseContextMenu::BaseContextMenu( QWidget* parent ):
    QMenu( parent ),
    Counter( "BaseContextMenu" ),
    hideDisabledActions_( false ),
    needSeparator_( false )
{}

//___________________________________________________
QAction* BaseContextMenu::addSeparator( void )
{
    separator_ = QMenu::addSeparator();
    if( hideDisabledActions_ )
    {
        needSeparator_ = true;
        separator_.data()->setVisible( false );
    }

    return separator_.data();
}

//___________________________________________________
QAction* BaseContextMenu::addMenu( QMenu* menu )
{
    if( hideDisabledActions_ && needSeparator_ && separator_ )
    {
        separator_.data()->setVisible( true );
        needSeparator_ = false;
    }

    return QMenu::addMenu( menu );

}

//___________________________________________________
void BaseContextMenu::addAction( QAction* action )
{
    if( hideDisabledActions_ )
    {

        if( !action->isEnabled() ) return;
        else if( needSeparator_ && separator_ )
        {
            separator_.data()->setVisible( true );
            needSeparator_ = false;
        }

        QMenu::addAction( action );
    }

}

//___________________________________________________
void BaseContextMenu::insertAction( QAction* before, QAction* action )
{

    // TODO: see if one needs to check pending separators
    if( action->isEnabled() || !hideDisabledActions_ )
    { QMenu::insertAction( before, action ); }

}

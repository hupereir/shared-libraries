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

#include "CustomMenu.h"

//_______________________________________________________
CustomMenu::CustomMenu( QWidget* parent ):
QMenu( parent ),
Counter( "CustomMenu" )
{}

//_______________________________________________________
CustomMenu::CustomMenu( const QString& title, QWidget* parent ):
QMenu( title, parent ),
Counter( "CustomMenu" )
{}

//_______________________________________________________
void CustomMenu::addInvisibleAction( QAction* action )
{
    QMenu::addAction( action );
    action->setVisible( false );
    invisibleActions_.append( action );
}

//_______________________________________________________
void CustomMenu::clear()
{
    QMenu::clear();
    invisibleActions_.clear();
}

//_______________________________________________________
void CustomMenu::keyPressEvent( QKeyEvent* event )
{

    if( event->key() == Qt::Key_Shift )
    {
        for( auto&& action:invisibleActions_ )
        {  if( action ) action.data()->setVisible( true ); }
    }

}

//_______________________________________________________
void CustomMenu::keyReleaseEvent( QKeyEvent* event )
{

    if( event->key() == Qt::Key_Shift )
    {
        for( auto&& action:invisibleActions_ )
        {  if( action ) action.data()->setVisible( false ); }
    }

}

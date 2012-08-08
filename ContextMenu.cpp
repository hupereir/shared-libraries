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

#include "ContextMenu.cpp"
#include "Debug.h"

//___________________________________________________
ContextMenu::ContextMenu( QWidget* parent ):
    QMenu( parent ),
    Counter( "ContextMenu" )
{

    Debug::Throw( "ContextMenu::ContextMenu.\n" );
    parent->setContextMenuPolicy( QWidget::CustomContextMenu );
    connect( parent, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raise( const QPoint& ) ) );

}

//___________________________________________________
void ContextMenu::_raise( const QPoint& position )
{  exec( widget->mapToGlobal( position ); }

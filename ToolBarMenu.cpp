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

/*!
  \file ToolBarMenu.cpp
  \brief provides toolbar manipulation menu
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "Debug.h"
#include "IconSizeMenu.h"
#include "ToolBarMenu.h"
#include "ToolButtonStyleMenu.h"

using namespace std;

//____________________________________________________
ToolBarMenu::ToolBarMenu( QWidget* parent ):
  QMenu( parent ),
  Counter( "ToolBarMenu" )
{

  Debug::Throw( "ToolBarMenu::ToolBarMenu.\n" );

  // make sure object is deleted when closing
  setAttribute( Qt::WA_DeleteOnClose, true );

  // add tool buttons and icon size submenus
  addMenu( toolbutton_style_menu_ = new ToolButtonStyleMenu( this ) );
  addMenu( icon_size_menu_ = new IconSizeMenu( this ) );

}

//____________________________________________________
ToolBarMenu::~ToolBarMenu( void )
{ Debug::Throw( "ToolBarMenu::~ToolBarMenu.\n" ); }

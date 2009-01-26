#ifndef ToolBarMenu_h
#define ToolBarMenu_h

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
  \file ToolBarMenu.h
  \brief provides toolbar manipulation menu
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QMenu>

#include "Counter.h"

class ToolButtonStyleMenu;
class IconSizeMenu;

//! provides toolbar manipulation menu
class ToolBarMenu: public QMenu, public Counter
{
    
  public:
    
  //! constructor
  ToolBarMenu( QWidget* parent = 0 );
    
  //! destructor
  virtual ~ToolBarMenu( void );
  
  //! tool button style menu
  ToolButtonStyleMenu &toolButtonStyleMenu( void ) const
  {
    assert( toolbutton_style_menu_ );
    return *toolbutton_style_menu_;
  }
  
  //! icon size menu
  IconSizeMenu &iconSizeMenu( void ) const
  {
    assert( icon_size_menu_ );
    return *icon_size_menu_;
  }

  private:
    
  //! tool button style
  ToolButtonStyleMenu* toolbutton_style_menu_;
 
  //! icon size
  IconSizeMenu* icon_size_menu_;
  
};

#endif

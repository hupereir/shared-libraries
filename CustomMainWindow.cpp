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
  \file CustomMainWindow.cpp
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#include "CustomMainWindow.h"
#include "CustomToolButton.h"

using namespace std;

//____________________________________________________________
CustomMainWindow::CustomMainWindow( QWidget *parent, Qt::WFlags wflags):
  QMainWindow( parent, wflags )
{ 
  Debug::Throw( "CustomMainWindow::CustomMainWindow.\n" );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
}

//____________________________________________________________
void CustomMainWindow::_updateConfiguration( void )
{ 
  
  Debug::Throw( "CustomMainWindow::_updateConfiguration.\n" );
  
  // pixmap size
  if( XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ) setIconSize( CustomToolButton::BigIconSize );
  else setIconSize( CustomToolButton::SmallIconSize );
  
  // text label for toolbars
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );
  
}


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
  \file CustomToolBar.cc
  \brief customized tool bar to handle position and visibility from Options
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>

#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________________________
std::map< std::string, Qt::ToolBarArea > CustomToolBar::toolbar_areas_;
bool CustomToolBar::initialized_ = CustomToolBar::_initializeAreas();

//_______________________________________________________________
CustomToolBar::CustomToolBar( const QString& title, QWidget* parent ):
  QToolBar( title, parent ),
  Counter( "CustomToolBar" ),
  lock_from_options_( true )
{
  Debug::Throw( "CustomToolBar::CustomToolBar.\n" );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateConfiguration() ) );
  updateConfiguration();
}

//_______________________________________________________________
CustomToolBar::CustomToolBar( QWidget* parent ):
  QToolBar( parent ),
  Counter( "CustomToolBar" ),
  lock_from_options_( true )
{
  Debug::Throw( "CustomToolBar::CustomToolBar.\n" );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateConfiguration() ) );
  updateConfiguration();
}

//_______________________________________________________________
void CustomToolBar::updateConfiguration( void )
{
  Debug::Throw( "CustomToolBar::updateConfiguration.\n" );
    
  // pixmap size
  if( XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ) setIconSize( CustomToolButton::BigIconSize );
  else setIconSize( CustomToolButton::SmallIconSize );
  
  // text label for toolbars
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );

  if( lock_from_options_ ) setMovable( !XmlOptions::get().get<bool>( "LOCK_TOOLBARS" ) );
}

//_______________________________________________________________
bool CustomToolBar::_initializeAreas( void )
{
  toolbar_areas_.insert( make_pair( "left", Qt::LeftToolBarArea ) );
  toolbar_areas_.insert( make_pair( "right", Qt::RightToolBarArea ) );
  toolbar_areas_.insert( make_pair( "top", Qt::TopToolBarArea ) );
  toolbar_areas_.insert( make_pair( "bottom", Qt::BottomToolBarArea ) );
  return true;
}

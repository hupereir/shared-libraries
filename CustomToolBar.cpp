
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
  \file CustomToolBar.cpp
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
  _installActions();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//_______________________________________________________________
CustomToolBar::CustomToolBar( QWidget* parent ):
  QToolBar( parent ),
  Counter( "CustomToolBar" ),
  lock_from_options_( true )
{
  Debug::Throw( "CustomToolBar::CustomToolBar.\n" );
  _installActions();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//_______________________________________________________________
void CustomToolBar::updateConfiguration( QMainWindow* parent, const CustomToolBar::List& toolbars )
{ 
  
  Debug::Throw( "CustomToolBar::updateConfiguration.\n" );

  // toolbars visibility and location
  for( List::const_iterator iter = toolbars.begin(); iter != toolbars.end(); iter++ )
  {
     
    QToolBar* toolbar( iter->first );
    string option_name( iter->second );
    string location_name( option_name + "_LOCATION" );
     
    bool visibility( XmlOptions::get().find( option_name ) ? XmlOptions::get().get<bool>( option_name ):true );
    bool current_visibility( toolbar->isVisible() );
    
    Qt::ToolBarArea location = (XmlOptions::get().find( location_name )) ? (Qt::ToolBarArea) CustomToolBar::nameToArea( XmlOptions::get().get<string>( location_name ) ):Qt::TopToolBarArea ;
    Qt::ToolBarArea current_location = parent->toolBarArea( toolbar );
    
    Debug::Throw() << "CustomToolBar::updateConfiguration - " << option_name << " visibility: " << visibility << " location: " << (int)location << endl;
    
    if( visibility )
    {
      if( !( current_visibility && (location == current_location) ) ) 
      {
        parent->addToolBar( location, toolbar );
        toolbar->show();
      }
    } else toolbar->hide();
    
    // update visibility action according to state for CustomToolbars
    CustomToolBar* custom_toolbar( dynamic_cast<CustomToolBar*>( toolbar ) );
    if( custom_toolbar ) custom_toolbar->visibilityAction().setChecked( visibility );
    
    // set options according to values
    XmlOptions::get().set<bool>( option_name, !toolbar->isHidden() );
    XmlOptions::get().set<string>( location_name, CustomToolBar::areaToName( parent->toolBarArea( toolbar ) ) );
    
  }  
}

//_______________________________________________________________
void CustomToolBar::saveConfiguration( QMainWindow* parent, const CustomToolBar::List& toolbars )
{ 
  Debug::Throw( "CustomToolBar::saveConfiguration.\n" );
  
  // save toolbars location and visibility
  for( List::const_iterator iter = toolbars.begin(); iter != toolbars.end(); iter++ )
  {
    
    QToolBar* toolbar( iter->first );
    string option_name( iter->second );
    string location_name( option_name + "_LOCATION" );
    XmlOptions::get().set<bool>( option_name, !toolbar->isHidden() );
    XmlOptions::get().set<string>( location_name, CustomToolBar::areaToName( parent->toolBarArea( toolbar ) ) );
  }
  
}
  
//_______________________________________________________________
void CustomToolBar::_updateConfiguration( void )
{
  Debug::Throw( "CustomToolBar::_updateConfiguration.\n" );
    
  // pixmap size
  if( XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ) setIconSize( CustomToolButton::BigIconSize );
  else setIconSize( CustomToolButton::SmallIconSize );
  
  // text label for toolbars
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );

  if( lock_from_options_ ) setMovable( !XmlOptions::get().get<bool>( "LOCK_TOOLBARS" ) );
}

//_______________________________________________________________
void CustomToolBar::_toggleVisibility( bool state )
{
  Debug::Throw( "CustomToolBar::_toggleVisibility.\n" );
  if( !state ) hide();
  else show();  
}

//_______________________________________________________________
void CustomToolBar::_installActions( void )
{
  Debug::Throw( "CustomToolBar::_installActions.\n" );
  visibility_action_ = new QAction( "Show &toolbar", this );
  visibility_action_->setCheckable( true );
  visibility_action_->setChecked( true );
  connect( visibility_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleVisibility( bool ) ) );
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

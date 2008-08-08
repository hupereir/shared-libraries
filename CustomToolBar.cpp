
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
CustomToolBar::CustomToolBar( const QString& title, QWidget* parent, const std::string& option_name ):
  QToolBar( title, parent ),
  Counter( "CustomToolBar" ),
  option_name_( option_name ),
  lock_from_options_( true )
{
  Debug::Throw( "CustomToolBar::CustomToolBar.\n" );
  _installActions();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( qApp, SIGNAL( saveConfiguration() ), SLOT( _saveConfiguration() ) );
  connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _saveConfiguration() ) );
  _updateConfiguration();
}

//_______________________________________________________________
CustomToolBar::~CustomToolBar( void )
{
  Debug::Throw( "~CustomToolBar::CustomToolBar.\n" );
  _saveConfiguration();
}

//_______________________________________________________________
void CustomToolBar::showEvent( QShowEvent* event )
{
  Debug::Throw() << "CustomToolBar::showEvent - name: " << option_name_ << endl;
  if( !isHidden() ) visibilityAction().setChecked( true ); 
  QToolBar::showEvent(event);
}

//_______________________________________________________________
void CustomToolBar::hideEvent( QHideEvent* event )
{
  
  Debug::Throw() << "CustomToolBar::hideEvent - name: " << option_name_ << endl;
  if( isHidden() ) visibilityAction().setChecked( false ); 
  QToolBar::hideEvent(event);

}

//_______________________________________________________________
void CustomToolBar::moveEvent( QMoveEvent* event )
{
  if( isFloating() || option_name_.empty() ) return QToolBar::moveEvent( event );
  
  QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
  if( parent )
  {
    string location_name( option_name_ + "_LOCATION" );
    Debug::Throw() << "CustomToolBar::moveEvent - " << location_name << ": " << CustomToolBar::areaToName( parent->toolBarArea( this ) ) << endl;
    XmlOptions::get().set<string>( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
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

  // lock
  if( lock_from_options_ ) setMovable( !XmlOptions::get().get<bool>( "LOCK_TOOLBARS" ) );

  // visibility
  bool visibility( XmlOptions::get().find( option_name_ ) ? XmlOptions::get().get<bool>( option_name_ ):true );
  bool current_visibility( isVisible() );
  
  // position
  // try cast parent to QMainWindow
  QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
  if( parent && !option_name_.empty() )
  {
    
    string location_name( option_name_ + "_LOCATION" );
    
    Qt::ToolBarArea location = (XmlOptions::get().find( location_name )) ? (Qt::ToolBarArea) nameToArea( XmlOptions::get().raw( location_name ) ):Qt::TopToolBarArea;
    Qt::ToolBarArea current_location = parent->toolBarArea( this );
    
    // some dump
    Debug::Throw() << "CustomToolBar::_updateConfiguration - " << option_name_ << " current_visibility: " << current_visibility << " current_location: " << areaToName( current_location ) << endl;
    Debug::Throw() << "CustomToolBar::_updateConfiguration - " << option_name_ << " visibility: " << visibility << " location: " << areaToName( location ) << endl;
    Debug::Throw() << endl;

    // set hidden if location is not specified
    if( location == Qt::NoToolBarArea ) visibility = false;
    
    // show toolbar
    if( visibility )
    { 
      
      if( !( current_visibility && (location == current_location) ) ) { parent->addToolBar( location, this ); }
      
    } else if( location != Qt::NoToolBarArea ) { 
      
      parent->addToolBar( location, this ); 
      hide();
      
    }
    
    // set options according to values
    XmlOptions::get().set<bool>( option_name_, !isHidden() );
    XmlOptions::get().set<string>( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
    
  }
    
  // update visibility action according to state for CustomToolbars
  Debug::Throw() << "CustomToolBar::_updateConfiguration - option_name: " << option_name_ << " visibility: " << visibility << endl;
  visibilityAction().setChecked( visibility );  

}

//_______________________________________________________________
void CustomToolBar::_saveConfiguration( void )
{ Debug::Throw( "CustomToolBar::_saveConfiguration.\n" ); }

//_______________________________________________________________
void CustomToolBar::_toggleVisibility( bool state )
{

  Debug::Throw() << "CustomToolBar::_toggleVisibility - name: " << option_name_ << " state: " << state << endl;
  if( !option_name_.empty() )
  {
    
    // save state
    XmlOptions::get().set<bool>( option_name_, state );
    
    // save position
    // try cast parent to QMainWindow
    QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
    if( parent )
    {
      string location_name( option_name_ + "_LOCATION" );
      XmlOptions::get().set<string>( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
    }
    
  }
  
  if( !state ) hide();
  else show();  
}

//_______________________________________________________________
void CustomToolBar::_installActions( void )
{
  Debug::Throw( "CustomToolBar::_installActions.\n" );
  ostringstream what;
  what << "&" << qPrintable( windowTitle() );
  visibility_action_ = new QAction( what.str().c_str(), this );
  visibility_action_->setCheckable( true );
  visibility_action_->setChecked( true );
  connect( visibility_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleVisibility( bool ) ) );
}

//_______________________________________________________________
bool CustomToolBar::_initializeAreas( void )
{
  toolbar_areas_.insert( make_pair( "none", Qt::NoToolBarArea ) );
  toolbar_areas_.insert( make_pair( "left", Qt::LeftToolBarArea ) );
  toolbar_areas_.insert( make_pair( "right", Qt::RightToolBarArea ) );
  toolbar_areas_.insert( make_pair( "top", Qt::TopToolBarArea ) );
  toolbar_areas_.insert( make_pair( "bottom", Qt::BottomToolBarArea ) );
  return true;
}

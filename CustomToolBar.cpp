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

#include "BaseMainWindow.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "Singleton.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________________________
CustomToolBar::AreaMap& CustomToolBar::_toolbarAreas( void )
{
  static AreaMap areas = _initializeAreas();
  return areas;
}

//_______________________________________________________________
CustomToolBar::CustomToolBar( const QString& title, QWidget* parent, const QString& option_name ):
  QToolBar( title, parent ),
  Counter( "CustomToolBar" ),
  option_name_( option_name ),
  size_from_options_( true ),
  lock_from_options_( true )
{
  Debug::Throw( "CustomToolBar::CustomToolBar.\n" );
  if( !option_name.isEmpty() ) setObjectName( option_name );
  _installActions();
  
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
}

//_______________________________________________________________
CustomToolBar::~CustomToolBar( void )
{ Debug::Throw( "~CustomToolBar::CustomToolBar.\n" ); }

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
  if( isFloating() || option_name_.isEmpty() ) return QToolBar::moveEvent( event );
  
  QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
  if( parent )
  {
    QString location_name( option_name_ + "_LOCATION" );
    XmlOptions::get().setRaw( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
  }
  
}

//_______________________________________________________________
void CustomToolBar::_toggleVisibility( bool state )
{

  Debug::Throw() << "CustomToolBar::_toggleVisibility - name: " << option_name_ << " state: " << state << endl;
  if( !option_name_.isEmpty() )
  {
    
    // save state
    XmlOptions::get().set<bool>( option_name_, state );
    
    // save position
    // try cast parent to QMainWindow
    QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
    if( parent )
    {
      QString location_name( option_name_ + "_LOCATION" );
      XmlOptions::get().setRaw( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
    }
    
  }

  // change visibility
  setVisible( state );
  
}

//_______________________________________________________________
void CustomToolBar::_updateConfiguration( void )
{
  Debug::Throw( "CustomToolBar::_updateConfiguration.\n" );
    
  // pixmap size
  if( sizeFromOptions() )
  {
    int icon_size( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
    if( icon_size <= 0 ) icon_size = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
    QToolBar::setIconSize( QSize( icon_size, icon_size ) );
  }
  
  // text label for toolbars
  setToolButtonStyle( (Qt::ToolButtonStyle) XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );

  // lock
  if( lockFromOptions() ) {
    
    BaseMainWindow* mainwindow( dynamic_cast<BaseMainWindow*>( window() ) );
    if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().find( mainwindow->lockToolBarsOptionName() ) )
    setMovable( !XmlOptions::get().get<bool>( mainwindow->lockToolBarsOptionName() ) );
    
  }
  
  // visibility
  bool current_visibility( isVisible() );
  bool visibility( (!option_name_.isEmpty() && XmlOptions::get().find( option_name_ ) ) ? XmlOptions::get().get<bool>( option_name_ ):current_visibility );
  
  // position
  // try cast parent to QMainWindow
  QMainWindow* parent( dynamic_cast<QMainWindow*>( parentWidget() ) );  
  if( parent && !option_name_.isEmpty() )
  {
    
    QString location_name( option_name_ + "_LOCATION" );
    
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
    XmlOptions::get().setRaw( location_name, CustomToolBar::areaToName( parent->toolBarArea( this ) ) );
    
  }
    
  // update visibility action according to state for CustomToolbars
  Debug::Throw() << "CustomToolBar::_updateConfiguration -"
    << " option_name: " << option_name_ 
    << " visibility: " << visibility << endl;
  
  visibilityAction().setChecked( visibility );  

}

//_______________________________________________________________
void CustomToolBar::_installActions( void )
{
  Debug::Throw( "CustomToolBar::_installActions.\n" );
  QString buffer;
  QTextStream( &buffer) << "&" << windowTitle();
  visibility_action_ = new QAction( buffer, this );
  visibility_action_->setCheckable( true );
  visibility_action_->setChecked( true );
  connect( visibility_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleVisibility( bool ) ) );
}

//_______________________________________________________________
CustomToolBar::AreaMap CustomToolBar::_initializeAreas( void )
{
  AreaMap out;
  out.insert( make_pair( "none", Qt::NoToolBarArea ) );
  out.insert( make_pair( "left", Qt::LeftToolBarArea ) );
  out.insert( make_pair( "right", Qt::RightToolBarArea ) );
  out.insert( make_pair( "top", Qt::TopToolBarArea ) );
  out.insert( make_pair( "bottom", Qt::BottomToolBarArea ) );
  return out;
}

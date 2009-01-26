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
  \file BaseMainWindow.cpp
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QToolBar>
#include <QWindowStateChangeEvent>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSizeMenu.h"
#include "BaseMainWindow.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "Singleton.h"
#include "ToolBarMenu.h"
#include "ToolButtonStyleMenu.h"
#include "QtUtil.h"

using namespace std;

//____________________________________________________________
BaseMainWindow::BaseMainWindow( QWidget *parent, Qt::WFlags wflags):
  QMainWindow( parent, wflags ),
  size_watcher_( this ),
  was_maximized_( false )
{ 
  Debug::Throw( "BaseMainWindow::BaseMainWindow.\n" );
  
  // lock toolbars action
  addAction( lock_toolbars_action_ = new QAction( IconEngine::get( ICONS::LOCK ), "&Lock toolbars", this ) );
  lockToolBarsAction().setCheckable( true );
  lockToolBarsAction().setChecked( false );
  connect( &lockToolBarsAction(), SIGNAL( toggled( bool ) ), SLOT( _lockToolBars( bool ) ) );
  
  // show menu action
  addAction( show_menu_action_ = new QAction( IconEngine::get( ICONS::SHOW_MENU ), "&Show menu bar", this ) );
  showMenuAction().setCheckable( true );
  showMenuAction().setChecked( true );
  showMenuAction().setShortcut( Qt::CTRL + Qt::Key_M );
  connect( &showMenuAction(), SIGNAL( toggled( bool ) ), SLOT( _showMenu( bool ) ) );
  
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( this, SIGNAL( toolbarConfigurationChanged() ), Singleton::get().application(), SIGNAL( configurationChanged() ) );
  _updateConfiguration();
    
}

//__________________________________________________
QSize BaseMainWindow::minimumSizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QMainWindow::minimumSizeHint();
}

//__________________________________________________
QSize BaseMainWindow::sizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QMainWindow::sizeHint();
}

//________________________________________________________________
void BaseMainWindow::centerOnDesktop( void )
{
  
  Debug::Throw( "BaseMainWindow::centerOnDesktop.\n" );
  move( QtUtil::centerOnDesktop( sizeHint() ) );
  
}

//________________________________________________________________
void BaseMainWindow::centerOnWidget( QWidget* parent )
{
  
  Debug::Throw( "BaseMainWindow::centerOnWidget.\n" );
  move( QtUtil::centerOnWidget( sizeHint(), parent ) );
  
}

//________________________________________________________________
QMenu* BaseMainWindow::createPopupMenu( void )
{
  Debug::Throw( "BaseMainWindow::createPopupMenu.\n" );
  if( !_hasToolBars() ) return QMainWindow::createPopupMenu();
  
  ToolBarMenu& menu = toolBarMenu( this );
  
  menu.toolButtonStyleMenu().select( (Qt::ToolButtonStyle) XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
  menu.iconSizeMenu().select( (IconSize::Size) XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
  menu.addAction( &showMenuAction() );
  connect( &menu.toolButtonStyleMenu(), SIGNAL( styleSelected( Qt::ToolButtonStyle ) ), SLOT( _updateToolButtonStyle( Qt::ToolButtonStyle ) ) );
  connect( &menu.iconSizeMenu(), SIGNAL( iconSizeSelected( IconSize::Size ) ), SLOT( _updateToolButtonIconSize( IconSize::Size ) ) );  
  return &menu;
  
}

//________________________________________________________________
ToolBarMenu& BaseMainWindow::toolBarMenu( QWidget* parent )
{
  
  ToolBarMenu* menu = new ToolBarMenu( parent );
  bool has_lockable_toolbars( installToolBarsActions( *menu->addMenu( "&ToolBars" ) ) );
  if( has_lockable_toolbars ) 
  {
    menu->addSeparator();
    menu->addAction( &lockToolBarsAction() );
  }
  
  return *menu;

}

//________________________________________________________________
bool BaseMainWindow::installToolBarsActions( QMenu& menu )
{
  Debug::Throw( "BaseMainWindow::installToolBarsActions.\n" );

  bool has_lockable_toolbars( false );
  QList<QToolBar*> toolbars( qFindChildren<QToolBar*>( this ) );
  for( QList<QToolBar*>::iterator iter = toolbars.begin(); iter != toolbars.end(); iter++ )
  {
    
    CustomToolBar* toolbar( dynamic_cast<CustomToolBar*>( *iter ) );
    if( toolbar ) { 
    
      Debug::Throw() << "BaseMainWindow::installToolBarsActions (custom) - " << qPrintable( (*iter)->windowTitle() ) << endl;
      menu.addAction( &toolbar->visibilityAction() ); 
    
    } else {
      
      // add visibility action
      Debug::Throw() << "BaseMainWindow::installToolBarsActions - " << qPrintable( (*iter)->windowTitle() ) << endl;
      QAction* action = new QAction( (*iter)->windowTitle(), &menu );
      action->setCheckable( true );
      action->setChecked( (*iter)->isVisible() );
      connect( action, SIGNAL( toggled( bool ) ), (*iter), SLOT( setVisible( bool ) ) );
      menu.addAction( action );
    
    }
    
    // skip if lockable toolbar was already found
    if( has_lockable_toolbars ) continue;
    
    // skip if parent is not this
    if( !((*iter)->parentWidget() == this) ) continue;
    
    // try cast to CustomToolBar and check for 'lock from options'
    if( toolbar && toolbar->lockFromOptions() ) continue;
    
    has_lockable_toolbars = true;
    
  }
  
  return has_lockable_toolbars;
  
}
 
//________________________________________________________________
void BaseMainWindow::uniconify( void )
{ 

  Debug::Throw( "BaseMainWindow::uniconify" );
  if( isMinimized() ) 
  {
    
    if( _wasMaximized() ) showMaximized();
    else showNormal();
    
  } else if( isHidden() ) show();
  
  activateWindow();
  raise();

}

//_______________________________________________________
bool BaseMainWindow::event( QEvent* event )
{
    
  // check that all needed widgets/actions are valid and checked.
  switch (event->type()) 
  {
          
    case QEvent::WindowStateChange:
    {
      // cast
      QWindowStateChangeEvent *state_event( static_cast<QWindowStateChangeEvent*>(event) );
      
      if( windowState() & Qt::WindowMinimized )
      { _setWasMaximized( state_event->oldState() & Qt::WindowMaximized ); }
      
    }
    break;
        
    default: break;
  }
  
  return QMainWindow::event( event );
  
}

//____________________________________________________________
void BaseMainWindow::resizeEvent( QResizeEvent* event )
{
  size_watcher_.restart(); 
  return QMainWindow::resizeEvent( event );
}
 
//________________________________________________________________
bool BaseMainWindow::_hasToolBars( void ) const
{
  Debug::Throw( "BaseMainWindow::_hasToolBars.\n" );
  return (bool) qFindChild<QToolBar*>( this );
}

//____________________________________________________________
void BaseMainWindow::_updateConfiguration( void )
{ 
  
  Debug::Throw( "BaseMainWindow::_updateConfiguration.\n" );
  
  // icon size
  setIconSize( IconSize( this, (IconSize::Size) XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) ) );
  
  // text label for toolbars
  setToolButtonStyle( (Qt::ToolButtonStyle) XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
  
  // toolbars locked
  if( XmlOptions::get().find( "LOCK_TOOLBARS" ) )
  { lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( "LOCK_TOOLBARS" ) ); }
  
  // menu visibility
  if( XmlOptions::get().find( "SHOW_MENU" ) )
  { showMenuAction().setChecked( XmlOptions::get().get<bool>( "SHOW_MENU" ) ); }
  
}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonStyle( Qt::ToolButtonStyle style )
{
  
  Debug::Throw( "BaseMainWindow::_updateToolButtonStyle.\n" );
  XmlOptions::get().set<int>( "TOOLBUTTON_TEXT_POSITION", (int)style );
  emit toolbarConfigurationChanged();
  
}

//____________________________________________________________
void BaseMainWindow::_updateToolButtonIconSize( IconSize::Size size )
{
  
  Debug::Throw( "BaseMainWindow::_updateToolButtonIconSize.\n" );
  XmlOptions::get().set<int>( "TOOLBUTTON_ICON_SIZE", size );
  emit toolbarConfigurationChanged();

}

//____________________________________________________________
void BaseMainWindow::_lockToolBars( bool value )
{
  Debug::Throw( "BaseMainWindow::_lockToolBars.\n" );
  QList<QToolBar*> toolbars( qFindChildren<QToolBar*>( this ) );
  for( QList<QToolBar*>::iterator iter = toolbars.begin(); iter != toolbars.end(); iter++ )
  {
    
    // skip if parent is not this
    if( !((*iter)->window() == this) ) continue;
    
    // try cast to CustomToolBar and check for 'lock from options'
    CustomToolBar* toolbar( dynamic_cast<CustomToolBar*>( *iter ) );
    if( toolbar && toolbar->lockFromOptions() ) continue;
    
    // update movability
    (*iter)->setMovable( !value );
  
  }
  
  XmlOptions::get().set<bool>( "LOCK_TOOLBARS", value );
  return;
}


//____________________________________________________________
void BaseMainWindow::_showMenu( bool value )
{
  Debug::Throw( "BaseMainWindow::_showMenu.\n" );
  
  if( !menuWidget() ) return;
  menuWidget()->setVisible( value );
  
  // save option
  XmlOptions::get().set<bool>( "SHOW_MENU", value );
  
}  

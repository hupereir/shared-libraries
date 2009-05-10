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
#include <QMenuBar>
#include <QTextStream>
#include <QToolBar>
#include <QWindowStateChangeEvent>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSizeMenu.h"
#include "BaseMainWindow.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "ToolBarMenu.h"
#include "ToolButtonStyleMenu.h"
#include "Util.h"

using namespace std;

//____________________________________________________________
BaseMainWindow::BaseMainWindow( QWidget *parent, Qt::WFlags wflags):
  QMainWindow( parent, wflags ),
  size_watcher_( this ),
  was_maximized_( false )
{ 
  Debug::Throw( "BaseMainWindow::BaseMainWindow.\n" );
  
  // lock toolbars action
  addAction( lock_toolbars_action_ = new QAction( IconEngine::get( ICONS::LOCK ), "&Lock Toolbars", this ) );
  lockToolBarsAction().setCheckable( true );
  lockToolBarsAction().setChecked( true );
  connect( &lockToolBarsAction(), SIGNAL( toggled( bool ) ), SLOT( _lockToolBars( bool ) ) );
  
  // show menu action
  addAction( show_menu_action_ = new QAction( IconEngine::get( ICONS::SHOW_MENU ), "&Show Menubar", this ) );
  showMenuAction().setCheckable( true );
  showMenuAction().setChecked( true );
  showMenuAction().setShortcut( Qt::CTRL + Qt::Key_M );
  showMenuAction().setEnabled( false );
  connect( &showMenuAction(), SIGNAL( toggled( bool ) ), SLOT( _showMenu( bool ) ) );
  
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( this, SIGNAL( toolbarConfigurationChanged() ), Singleton::get().application(), SIGNAL( configurationChanged() ) );
  _updateConfiguration();
    
}

//__________________________________________________
void BaseMainWindow::setOptionName( const QString& name )
{ 
  
  Debug::Throw( "BaseMainWindow::setOptionName.\n" );
  size_watcher_.setOptionName( name ); 
  
  if( name.isEmpty() ) {
  
    lock_toolbars_option_name_.clear();
    show_menu_option_name_.clear();

  } else {
   
    lock_toolbars_option_name_ = name+"_LOCK_TOOLBARS";
    if( !XmlOptions::get().find( lockToolBarsOptionName() ) ) XmlOptions::get().set<bool>( lockToolBarsOptionName(), lockToolBarsAction().isChecked() );
    else { lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName() ) ); }

    show_menu_option_name_ = name+"_SHOW_MENU";
    if( !XmlOptions::get().find( showMenuOptionName() ) ) XmlOptions::get().set<bool>( showMenuOptionName(), showMenuAction().isChecked() );
    else showMenuAction().setChecked( XmlOptions::get().get<bool>( showMenuOptionName() ) );
    
  }
  
}

//__________________________________________________
void BaseMainWindow::setWindowTitle( const QString& title )
{
  
  Debug::Throw( "BaseMainWindow::setWindowTitle.\n" );  
  QMainWindow::setWindowTitle( Util::windowTitle( title ) );
  
}

//__________________________________________________
void BaseMainWindow::setMenuBar( QMenuBar* menu )
{
  
  Debug::Throw( "BaseMainWindow::setMenuBar.\n" );
  QMainWindow::setMenuBar( menu );
  if( !menuBar() ) return;
  menuBar()->setVisible( showMenuAction().isChecked() );
  showMenuAction().setEnabled( true );
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
  if( !_hasToolBars() ) 
  {

    QMenu* menu = new QMenu( this );
    menu->addAction(&showMenuAction() );
    return menu;
    
  } else {
  
    ToolBarMenu& menu = toolBarMenu( this );
    menu.toolButtonStyleMenu().select( (Qt::ToolButtonStyle) XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
    menu.iconSizeMenu().select( (IconSize::Size) XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
    connect( &menu.toolButtonStyleMenu(), SIGNAL( styleSelected( Qt::ToolButtonStyle ) ), SLOT( _updateToolButtonStyle( Qt::ToolButtonStyle ) ) );
    connect( &menu.iconSizeMenu(), SIGNAL( iconSizeSelected( IconSize::Size ) ), SLOT( _updateToolButtonIconSize( IconSize::Size ) ) );  
    return &menu;
  
  }
  
}

//________________________________________________________________
ToolBarMenu& BaseMainWindow::toolBarMenu( QWidget* parent )
{
  
  Debug::Throw( "BaseMainWindow::toolBarMenu.\n" );
  ToolBarMenu* menu = new ToolBarMenu( parent );
  
  bool has_lockable_toolbars( installToolBarsActions( *menu->addMenu( "&ToolBars" ) ) );
  if( has_lockable_toolbars ) 
  {
    menu->addSeparator();
    menu->addAction( &lockToolBarsAction() );
  }
  
  // show/hide menu
  menu->addAction( &showMenuAction() );

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
    
      Debug::Throw() << "BaseMainWindow::installToolBarsActions (custom) - " << (*iter)->windowTitle() << endl;
      menu.addAction( &toolbar->visibilityAction() ); 
    
    } else {
      
      // add visibility action
      Debug::Throw() << "BaseMainWindow::installToolBarsActions - " << (*iter)->windowTitle() << endl;
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
  if( hasOptionName() )
  {
    
    // toolbars locked
    if( XmlOptions::get().find( lockToolBarsOptionName() ) )
    { lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( lockToolBarsOptionName() ) ); }
  
    // menu visibility
    if( XmlOptions::get().find( showMenuOptionName() ) )
    { showMenuAction().setChecked( XmlOptions::get().get<bool>( showMenuOptionName() ) ); }
    
  }
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
  
  if( hasOptionName() ) XmlOptions::get().set<bool>( lockToolBarsOptionName(), value );
  return;
}


//____________________________________________________________
void BaseMainWindow::_showMenu( bool value )
{
  Debug::Throw( "BaseMainWindow::_showMenu.\n" );
  
  if( !menuWidget() ) return;
  menuWidget()->setVisible( value );
  
  // save option
  if( hasOptionName() ) XmlOptions::get().set<bool>( showMenuOptionName(), value );
  
}  

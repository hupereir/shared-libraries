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

#include <assert.h>
#include <QApplication>
#include <QToolBar>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "CustomMainWindow.h"
#include "CustomToolBar.h"
#include "CustomToolButton.h"

using namespace std;

//____________________________________________________________
CustomMainWindow::CustomMainWindow( QWidget *parent, Qt::WFlags wflags):
  QMainWindow( parent, wflags ),
  size_watcher_( this )
{ 
  Debug::Throw( "CustomMainWindow::CustomMainWindow.\n" );
  
  // lock toolbars action
  addAction( lock_toolbars_action_ = new QAction( IconEngine::get( ICONS::LOCK ), "&Lock toolbars", this ) );
  lockToolBarsAction().setCheckable( true );
  lockToolBarsAction().setChecked( false );
  connect( &lockToolBarsAction(), SIGNAL( toggled( bool ) ), SLOT( _lockToolBars( bool ) ) );
  
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( this, SIGNAL( toolbarConfigurationChanged() ), qApp, SIGNAL( configurationChanged() ) );
  _updateConfiguration();
  
  toolbutton_style_group_ = new QActionGroup( this );
  toolbutton_style_group_->setExclusive( true );
  connect( toolbutton_style_group_, SIGNAL( triggered( QAction* ) ), SLOT( _updateToolButtonStyle( QAction* ) ) );

  icon_size_group_ = new QActionGroup( this );
  icon_size_group_->setExclusive( true );
  connect( icon_size_group_, SIGNAL( triggered( QAction* ) ), SLOT( _updateToolButtonIconSize( QAction* ) ) );
  
}

//__________________________________________________
QSize CustomMainWindow::minimumSizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QMainWindow::minimumSizeHint();
}

//__________________________________________________
QSize CustomMainWindow::sizeHint( void ) const
{
  QSize out( size_watcher_.sizeHint() );
  return out.isValid() ? out:QMainWindow::sizeHint();
}

//________________________________________________________________
QMenu* CustomMainWindow::createPopupMenu( void )
{
  Debug::Throw( "CustomMainWindow::createPopupMenu.\n" );
  if( !_hasToolBars() ) return QMainWindow::createPopupMenu();
  
  QMenu* menu = new QMenu();
  QMenu* submenu;
  _installToolButtonStyleActions( *(submenu = menu->addMenu( "&Text position" )) );
  _installToolButtonIconSizeActions( *(submenu = menu->addMenu( "&Icon size" )) );
  bool has_lockable_toolbars( installToolBarsActions( *(submenu = menu->addMenu( "&ToolBars" )) ) );
  if( has_lockable_toolbars ) 
  {
    menu->addSeparator();
    menu->addAction( &lockToolBarsAction() );
  }
  
  return menu;
  
}
 
//________________________________________________________________
bool CustomMainWindow::installToolBarsActions( QMenu& menu )
{
  Debug::Throw( "CustomMainWindow::installToolBarsActions.\n" );

  bool has_lockable_toolbars( false );
  QList<QToolBar*> toolbars( qFindChildren<QToolBar*>( this ) );
  for( QList<QToolBar*>::iterator iter = toolbars.begin(); iter != toolbars.end(); iter++ )
  {
    
    CustomToolBar* toolbar( dynamic_cast<CustomToolBar*>( *iter ) );
    if( toolbar ) { menu.addAction( &toolbar->visibilityAction() ); }
    else {
      
      // add visibility action
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
  
//____________________________________________________________
void CustomMainWindow::resizeEvent( QResizeEvent* event )
{
  size_watcher_.restart(); 
  return QMainWindow::resizeEvent( event );
}
 
//________________________________________________________________
bool CustomMainWindow::_hasToolBars( void ) const
{
  Debug::Throw( "CustomMainWindow::_hasToolBars.\n" );
  return (bool) qFindChild<QToolBar*>( this );
}

//________________________________________________________________
void CustomMainWindow::_installToolButtonStyleActions( QMenu& menu )
{
  Debug::Throw( "CustomMainWindow::_installToolButtonStyleActions.\n" );
  
  // clear map
  toolbutton_style_actions_.clear();

  // install values
  typedef list< std::pair<QString, int > > List;
  static List action_names;
  if( action_names.empty() )
  {
    action_names.push_back( make_pair( "&Icons Only", Qt::ToolButtonIconOnly ) );
    action_names.push_back( make_pair( "&Text Only", Qt::ToolButtonTextOnly ) );
    action_names.push_back( make_pair( "Text &Alongside icons", Qt::ToolButtonTextBesideIcon ) );
    action_names.push_back( make_pair( "Text &Under icons", Qt::ToolButtonTextUnderIcon ) );
  }
  
  // generic action
  int position = XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" );
  for( List::const_iterator iter = action_names.begin(); iter != action_names.end(); iter++ )
  {
    QAction* action = new QAction( iter->first, &menu );
    menu.addAction( action );
    action->setCheckable( true );
    if( position == iter->second ) action->setChecked( true );
    toolbutton_style_actions_.insert( make_pair( action, iter->second ) );
    toolbutton_style_group_->addAction( action );
  }
  
  return; 
}
 
//________________________________________________________________
void CustomMainWindow::_installToolButtonIconSizeActions( QMenu& menu )
{
  Debug::Throw( "CustomMainWindow::_installToolButtonIconSizeActions.\n" );

  // clear map
  icon_size_actions_.clear();
  
  const CustomToolButton::IconSizeMap& sizes( CustomToolButton::iconSizes() );

  // generic action
  int size = XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" );
  for( CustomToolButton::IconSizeMap::const_iterator iter = sizes.begin(); iter != sizes.end(); iter++ )
  {
    QAction* action = new QAction( iter->second, &menu );
    menu.addAction( action );
    action->setCheckable( true );
    if( size == iter->first ) action->setChecked( true );
    icon_size_actions_.insert( make_pair( action, iter->first ) );
    icon_size_group_->addAction( action );
  }

  return; 
}

//____________________________________________________________
void CustomMainWindow::_updateConfiguration( void )
{ 
  
  Debug::Throw( "CustomMainWindow::_updateConfiguration.\n" );
  
  // icon size
  int icon_size( XmlOptions::get().get<int>( "TOOLBUTTON_ICON_SIZE" ) );
  if( icon_size <= 0 ) icon_size = style()->pixelMetric( QStyle::PM_ToolBarIconSize );
  setIconSize( QSize( icon_size, icon_size ) );
  
  // text label for toolbars
  setToolButtonStyle( (Qt::ToolButtonStyle) XmlOptions::get().get<int>( "TOOLBUTTON_TEXT_POSITION" ) );
  
  // toolbars locked
  lockToolBarsAction().setChecked( XmlOptions::get().get<bool>( "LOCK_TOOLBARS" ) );
  
}

//____________________________________________________________
void CustomMainWindow::_updateToolButtonStyle( QAction* action )
{
  
  Debug::Throw( "CustomMainWindow::_updateToolButtonStyle.\n" );
  
  // find matching actions
  ActionMap::const_iterator iter = toolbutton_style_actions_.find( action );
  assert( iter != toolbutton_style_actions_.end() );
  
  // update options
  XmlOptions::get().set<int>( "TOOLBUTTON_TEXT_POSITION", (int)iter->second );
  emit toolbarConfigurationChanged();
  
}

//____________________________________________________________
void CustomMainWindow::_updateToolButtonIconSize( QAction* action )
{
  
  Debug::Throw( "CustomMainWindow::_updateToolButtonIconSize.\n" );
 
  // find matching actions
  ActionMap::const_iterator iter = icon_size_actions_.find( action );
  assert( iter != icon_size_actions_.end() );
  
  // update options
  XmlOptions::get().set<int>( "TOOLBUTTON_ICON_SIZE", (int)iter->second );
  emit toolbarConfigurationChanged();

}

//____________________________________________________________
void CustomMainWindow::_lockToolBars( bool value )
{
  Debug::Throw( "CustomMainWindow::_lockToolBars.\n" );
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
  
  XmlOptions::get().set<bool>( "LOCK_TOOLBARS", lockToolBarsAction().isChecked() );
  return;
}

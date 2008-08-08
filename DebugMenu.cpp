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
  \file DebugMenu.cpp
  \brief some basic runtime debuging
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>

#include "CounterDialog.h"
#include "Debug.h"
#include "DebugMenu.h"
#include "IconCacheDialog.h"
#include "QtUtil.h"
#include "OptionDialog.h"
#include "SystemEnvironmentDialog.h"

using namespace std;

//_______________________________________________________
DebugMenu::DebugMenu( QWidget* parent, unsigned int flags ):
  QMenu( parent ),
  Counter( "DebugMenu" ),
  counter_dialog_( 0 ),
  icon_cache_dialog_( 0 )
{
  
  Debug::Throw( "DebugMenu::DebugMenu.\n" );
  if( flags&COUNTERS ) addAction( "Object &Counters ", this, SLOT( _showCounterDialog() ) );
  if( flags&ICONS ) addAction( "&Icon Cache ", this, SLOT( _showIconCacheDialog() ) );
  if( flags&SYSTEM ) addAction( "&System environment ", this, SLOT( _showSystemEnvironment() ) );
  if( flags&OPTIONS ) addAction( "&Run-time options", this, SLOT( _showOptions() ) );
}  
  
//_______________________________________________
void DebugMenu::_showCounterDialog( void )
{
  Debug::Throw( "DebugMenu::_ShowCounterDialog.\n" );
  
  // check counter dialog has been build
  if( !counter_dialog_ ) {
    
    counter_dialog_ = new CounterDialog( parentWidget() );
    counter_dialog_->update();
    QtUtil::centerOnParent( counter_dialog_ );
    counter_dialog_->show();
    
  } else {
    
    counter_dialog_->update();
    QtUtil::centerOnParent( counter_dialog_ );
    counter_dialog_->show();
    QtUtil::uniconify( counter_dialog_ );
  
  }
  
}

//_______________________________________________
void DebugMenu::_showIconCacheDialog( void )
{
  Debug::Throw( "DebugMenu::_ShowIconCacheDialog.\n" );
  
  // check IconCache dialog has been build
  if( !icon_cache_dialog_ ) {
    
    icon_cache_dialog_ = new IconCacheDialog( parentWidget() );
    icon_cache_dialog_->update();
    QtUtil::centerOnParent( icon_cache_dialog_ );
    icon_cache_dialog_->show();
    
  } else {
    
    icon_cache_dialog_->update();
    QtUtil::centerOnParent( icon_cache_dialog_ );
    icon_cache_dialog_->show();
    QtUtil::uniconify( icon_cache_dialog_ );
  
  }
  
}

//_______________________________________________
void DebugMenu::_showSystemEnvironment( void )
{
  Debug::Throw( "DebugMenu::_showSystemEnvironment.\n" );
  
  SystemEnvironmentDialog* dialog = new SystemEnvironmentDialog( parentWidget() );
  QtUtil::centerOnParent( dialog );
  dialog->show();
  return;
  
}

//_______________________________________________
void DebugMenu::_showOptions( void )
{
  Debug::Throw( "DebugMenu::_showOptions.\n" );
  
  OptionDialog* dialog = new OptionDialog( parentWidget() );
  QtUtil::centerOnParent( dialog );
  dialog->show();
  return;
  
}

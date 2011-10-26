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
#include "PixmapCacheDialog.h"
#include "OptionDialog.h"
#include "SystemEnvironmentDialog.h"



//_______________________________________________________
DebugMenu::DebugMenu( QWidget* parent, unsigned int flags ):
  QMenu( parent ),
  Counter( "DebugMenu" ),
  counter_dialog_( 0 ),
  pixmap_cache_dialog_( 0 )
{

  Debug::Throw( "DebugMenu::DebugMenu.\n" );
  if( flags&COUNTERS ) addAction( "Object &Counters ", this, SLOT( _showCounterDialog() ) );
  if( flags&ICONS ) addAction( "&Icon Cache ", this, SLOT( _showPixmapCacheDialog() ) );
  if( flags&SYSTEM ) addAction( "&System environment ", this, SLOT( _showSystemEnvironment() ) );
  if( flags&OPTIONS ) addAction( "&Run-time options", this, SLOT( _showOptions() ) );
}

//_______________________________________________
void DebugMenu::_showCounterDialog( void )
{
  Debug::Throw( "DebugMenu::_ShowCounterDialog.\n" );

  // check counter dialog has been build
  if( !counter_dialog_ ) {

    counter_dialog_ = new CounterDialog( qApp->activeWindow() );
    counter_dialog_->centerOnWidget( qApp->activeWindow() );
    counter_dialog_->update();
    counter_dialog_->show();

  } else {

    counter_dialog_->update();
    counter_dialog_->centerOnWidget( qApp->activeWindow() );
    counter_dialog_->show();
    counter_dialog_->uniconify();

  }

}

//_______________________________________________
void DebugMenu::_showPixmapCacheDialog( void )
{
  Debug::Throw( "DebugMenu::_ShowPixmapCacheDialog.\n" );

  // check PixmapCache dialog has been build
  if( !pixmap_cache_dialog_ ) {

    pixmap_cache_dialog_ = new PixmapCacheDialog( qApp->activeWindow() );
    pixmap_cache_dialog_->centerOnWidget( qApp->activeWindow() );
    pixmap_cache_dialog_->update();
    pixmap_cache_dialog_->show();

  } else {

    pixmap_cache_dialog_->update();
    pixmap_cache_dialog_->centerOnWidget( qApp->activeWindow() );
    pixmap_cache_dialog_->show();
    pixmap_cache_dialog_->uniconify();

  }

}

//_______________________________________________
void DebugMenu::_showSystemEnvironment( void )
{
  Debug::Throw( "DebugMenu::_showSystemEnvironment.\n" );

  SystemEnvironmentDialog* dialog = new SystemEnvironmentDialog( qApp->activeWindow() );
  dialog->centerOnWidget( qApp->activeWindow() );
  dialog->show();
  return;

}

//_______________________________________________
void DebugMenu::_showOptions( void )
{
  Debug::Throw( "DebugMenu::_showOptions.\n" );

  OptionDialog* dialog = new OptionDialog( qApp->activeWindow() );
  dialog->centerOnWidget( qApp->activeWindow() );
  dialog->show();
  return;

}

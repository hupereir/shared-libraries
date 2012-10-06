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

#include "CounterDialog.h"
#include "Debug.h"
#include "DebugMenu.h"
#include "IconCacheDialog.h"
#include "OptionDialog.h"
#include "SystemEnvironmentDialog.h"

#include <QtGui/QApplication>

//_______________________________________________________
DebugMenu::DebugMenu( QWidget* parent, unsigned int flags ):
    QMenu( parent ),
    Counter( "DebugMenu" ),
    counterDialog_( 0 ),
    iconCacheDialog_( 0 )
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
    if( !counterDialog_ ) {

        counterDialog_ = new CounterDialog( qApp->activeWindow() );
        counterDialog_->centerOnWidget( qApp->activeWindow() );
        counterDialog_->update();
        counterDialog_->show();

    } else {

        counterDialog_->update();
        counterDialog_->centerOnWidget( qApp->activeWindow() );
        counterDialog_->show();
        counterDialog_->uniconify();

    }

}

//_______________________________________________
void DebugMenu::_showIconCacheDialog( void )
{
    Debug::Throw( "DebugMenu::_ShowIconCacheDialog.\n" );

    // check IconCache dialog has been build
    if( !iconCacheDialog_ ) {

        iconCacheDialog_ = new IconCacheDialog( qApp->activeWindow() );
        iconCacheDialog_->centerOnWidget( qApp->activeWindow() );
        iconCacheDialog_->update();
        iconCacheDialog_->show();

    } else {

        iconCacheDialog_->update();
        iconCacheDialog_->centerOnWidget( qApp->activeWindow() );
        iconCacheDialog_->show();
        iconCacheDialog_->uniconify();

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

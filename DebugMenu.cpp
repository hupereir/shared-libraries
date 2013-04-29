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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "CounterDialog.h"
#include "Debug.h"
#include "DebugMenu.h"
#include "IconCacheDialog.h"
#include "OptionDialog.h"
#include "SystemEnvironmentDialog.h"

#include <QApplication>
#include <QKeyEvent>

//_______________________________________________________
DebugMenu::DebugMenu( QWidget* parent, Flags flags ):
    QMenu( parent ),
    Counter( "DebugMenu" ),
    counterDialog_( 0 ),
    iconCacheDialog_( 0 )
{

    setTitle( tr( "Debug" ) );

    Debug::Throw( "DebugMenu::DebugMenu.\n" );
    if( flags&Counters ) addAction( tr("Object Counters"), this, SLOT( _showCounterDialog() ) );
    if( flags&Icons ) addAction( tr("Icon Cache"), this, SLOT( _showIconCacheDialog() ) );
    if( flags&System ) addAction( tr("System environment"), this, SLOT( _showSystemEnvironment() ) );
    if( flags&Options ) addAction( tr("Run-time options"), this, SLOT( _showOptions() ) );

    if( qobject_cast<QMenu*>( parent ) ) parent->installEventFilter( this );

}

//_______________________________________________
bool DebugMenu::eventFilter( QObject* object, QEvent* event )
{

    // check object
    if( object == parentWidget() )
    {
        switch( event->type() )
        {
            case QEvent::KeyPress:
            if( static_cast<QKeyEvent*>( event )->key() == Qt::Key_Shift )
            { menuAction()->setVisible( true ); }
            break;

            case QEvent::KeyRelease:
            if( static_cast<QKeyEvent*>( event )->key() == Qt::Key_Shift )
            { menuAction()->setVisible( false ); }
            break;

            case QEvent::Show:
            menuAction()->setVisible( false );

            default: break;
        }

    }

    return QMenu::eventFilter( object, event );

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

        iconCacheDialog_ = new IconCacheDialog( this );
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

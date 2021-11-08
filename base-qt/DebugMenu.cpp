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

#include "DebugMenu.h"
#include "CounterDialog.h"
#include "Debug.h"
#include "IconCacheDialog.h"
#include "OptionDialog.h"
#include "SystemEnvironmentDialog.h"

#include <QApplication>
#include <QKeyEvent>

//_______________________________________________________
DebugMenu::DebugMenu( QWidget* parent, Flags flags ):
    QMenu( parent ),
    Counter( QStringLiteral("DebugMenu") )
{

    setTitle( tr( "Debug" ) );

    Debug::Throw( QStringLiteral("DebugMenu::DebugMenu.\n") );
    if( flags&Flag::Counters ) addAction( tr("Object Counters"), this, &DebugMenu::_showCounterDialog );
    if( flags&Flag::Icons ) addAction( tr("Icon Cache"), this, &DebugMenu::_showIconCacheDialog );
    if( flags&Flag::System ) addAction( tr("System environment"), this, &DebugMenu::_showSystemEnvironment );
    if( flags&Flag::Options ) addAction( tr("Run-time options"), this, &DebugMenu::_showOptions );

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
void DebugMenu::_showCounterDialog()
{
    Debug::Throw( QStringLiteral("DebugMenu::_ShowCounterDialog.\n") );

    // check counter dialog has been build
    if( !counterDialog_ ) {

        counterDialog_ = new CounterDialog( qApp->activeWindow() );
        counterDialog_->centerOnWidget( qApp->activeWindow() );
        counterDialog_->updateCounters();
        counterDialog_->show();

    } else {

        counterDialog_->centerOnWidget( qApp->activeWindow() );
        counterDialog_->updateCounters();
        counterDialog_->show();
        counterDialog_->uniconify();

    }

}

//_______________________________________________
void DebugMenu::_showIconCacheDialog()
{
    Debug::Throw( QStringLiteral("DebugMenu::_ShowIconCacheDialog.\n") );

    // check IconCache dialog has been build
    if( !iconCacheDialog_ ) {

        iconCacheDialog_ = new IconCacheDialog( this );
        iconCacheDialog_->centerOnWidget( qApp->activeWindow() );
        iconCacheDialog_->updateCache();
        iconCacheDialog_->show();

    } else {

        iconCacheDialog_->centerOnWidget( qApp->activeWindow() );
        iconCacheDialog_->updateCache();
        iconCacheDialog_->show();
        iconCacheDialog_->uniconify();

    }

}

//_______________________________________________
void DebugMenu::_showSystemEnvironment()
{
    Debug::Throw( QStringLiteral("DebugMenu::_showSystemEnvironment.\n") );

    auto&& dialog = new SystemEnvironmentDialog( qApp->activeWindow() );
    dialog->centerOnWidget( qApp->activeWindow() );
    dialog->show();
    return;

}

//_______________________________________________
void DebugMenu::_showOptions()
{
    Debug::Throw( QStringLiteral("DebugMenu::_showOptions.\n") );

    auto&& dialog = new OptionDialog( qApp->activeWindow() );
    dialog->centerOnWidget( qApp->activeWindow() );
    dialog->show();
    return;

}

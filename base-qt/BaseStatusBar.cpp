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

#include "BaseStatusBar.h"
#include "ClockLabel.h"
#include "Debug.h"
#include "QtUtil.h"
#include "XmlOptions.h"

#include <QMainWindow>
#include <QMenu>

#include <memory>

//___________________________________________
BaseStatusBar::BaseStatusBar( QWidget* parent ):
    QStatusBar( parent ),
    Counter( QStringLiteral("BaseStatusBar") )
{
    Debug::Throw( QStringLiteral("BaseStatusBar::BaseStatusBar.\n") );
    setSizeGripEnabled( false );

    // if no size grip, still leaves some space to the right of the widget
    if( !isSizeGripEnabled() )
    { setContentsMargins( contentsMargins() + QMargins( 2, 0, 2, 0 )); }
}


//___________________________________________
void BaseStatusBar::addClock()
{
    Debug::Throw( QStringLiteral("BaseStatusBar::addClock.\n") );
    addPermanentWidget( new ClockLabel( this ) );
}

//____________________________________________
void BaseStatusBar::addLabel( int stretch )
{
    Debug::Throw( QStringLiteral("BaseStatusBar::addLabel.\n") );

    auto label = new StatusBarLabel( this );
    QtUtil::setMargin(label, 2);

    addPermanentWidget( label, stretch );
    labels_.append( label );
}

//__________________________________________________________________
void BaseStatusBar::contextMenuEvent( QContextMenuEvent *event )
{
    Debug::Throw( QStringLiteral("BaseStatusBar::contextMenuEvent.\n") );

    // get main window
    auto mainWindow = qobject_cast<QMainWindow*>( window() );
    if( !mainWindow ) return;

    // get menu
    std::unique_ptr<QMenu> menu( mainWindow->createPopupMenu() );
    if( menu ) menu->exec( event->globalPos() );
}

//__________________________________________________________________
void StatusBarLabel::setTextAndUpdate( const QString& message )
{
    QLabel::setText( message );
    qApp->processEvents();
}

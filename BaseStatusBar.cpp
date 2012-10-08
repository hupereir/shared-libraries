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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "BaseStatusBar.h"
#include "TransitionWidget.h"
#include "ClockLabel.h"
#include "Debug.h"
#include "XmlOptions.h"

#include <QtGui/QMainWindow>
#include <QtGui/QMenu>

//___________________________________________
BaseStatusBar::BaseStatusBar( QWidget* parent ):
    QStatusBar( parent ),
    Counter( "BaseStatusBar" )
{
    Debug::Throw( "BaseStatusBar::BaseStatusBar.\n" );
    assert( qobject_cast<QMainWindow*>( parent ) );
    setSizeGripEnabled( false );

    // if no size grip, still leaves some space to the right of the widget
    if( !isSizeGripEnabled() )
    {
        int left, right, top, bottom;
        getContentsMargins( &left, &top, &right, &bottom );
        setContentsMargins( left+2, top, right + 2, bottom );
    }

}


//___________________________________________
void BaseStatusBar::addClock( void )
{
    Debug::Throw( "BaseStatusBar::addClock.\n" );
    ClockLabel* clock = new ClockLabel( this );
    addPermanentWidget( clock );
}

//____________________________________________
void BaseStatusBar::addLabel( const int& stretch, bool animated )
{
    Debug::Throw( "BaseStatusBar::addLabel.\n" );

    StatusBarLabel* label(new StatusBarLabel( this ) );
    if( !animated )
    {
        label->transitionWidget().setEnableOnOptions( false );
        label->transitionWidget().setEnabled( false );
    }

    label->setMargin(2);
    addPermanentWidget( label, stretch );
    labels_ << label;
}

//__________________________________________________________________
void BaseStatusBar::contextMenuEvent( QContextMenuEvent *event )
{
    Debug::Throw( "BaseStatusBar::contextMenuEvent.\n" );
    QMenu* menu( static_cast<QMainWindow*>(parent())->createPopupMenu() );
    if( !menu ) return;
    menu->exec( event->globalPos() );
    menu->deleteLater();
}

//__________________________________________________________________
void StatusBarLabel::setTextAndUpdate( const QString& message )
{
    AnimatedLabel::setText( message );
    qApp->processEvents();
}

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

/*!
\file StatusBar.cpp
\brief  customized line edit for application state
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QMainWindow>
#include <QMenu>

#include "StatusBar.h"
#include "TransitionWidget.h"
#include "ClockLabel.h"
#include "Debug.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________
StatusBar::StatusBar( QWidget* parent ):
    QStatusBar( parent ),
    Counter( "StatusBar" )
{
    Debug::Throw( "StatusBar::StatusBar.\n" );
    assert( qobject_cast<QMainWindow*>( parent ) );
    setSizeGripEnabled( XmlOptions::get().get<bool>( "SIZE_GRIP_ENABLED" ) );

    // if no size grip, still leaves some space to the right of the widget
    if( !isSizeGripEnabled() )
    {
        int left, right, top, bottom;
        getContentsMargins( &left, &top, &right, &bottom );
        setContentsMargins( left+2, top, right + 2, bottom );
    }

}


//___________________________________________
void StatusBar::addClock( void )
{
    Debug::Throw( "StatusBar::addClock.\n" );
    ClockLabel* clock = new ClockLabel( this );
    addPermanentWidget( clock );
}

//____________________________________________
void StatusBar::addLabel( const int& stretch, bool animated )
{
    Debug::Throw( "StatusBar::addLabel.\n" );

    StatusBarLabel* label(new StatusBarLabel( this ) );
    if( !animated )
    {
        label->transitionWidget().setEnableOnOptions( false );
        label->transitionWidget().setEnabled( false );
    }

    label->setMargin(2);
    addPermanentWidget( label, stretch );
    labels_.push_back( label );
}

//__________________________________________________________________
void StatusBar::contextMenuEvent( QContextMenuEvent *event )
{
    Debug::Throw( "StatusBar::contextMenuEvent.\n" );
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

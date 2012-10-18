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
\file CursorMonitor.cpp
\brief used to automatically save size of top level widgets
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cassert>

#include "XmlOptions.h"
#include "CursorMonitor.h"



//_________________________________________________________
CursorMonitor::CursorMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( "CursorMonitor" ),
    enabled_( true ),
    autoHideDelay_( 1000 ),
    cursorState_( CursorVisible ),
    savedCursorShape_( parent->cursor().shape() )
{
    parent->setMouseTracking( true );
    parent->installEventFilter(this);
}

//_________________________________________________________
bool CursorMonitor::eventFilter( QObject* o, QEvent* e )
{
    if( !( o && o==parent() ) ) return false;
    if( !enabled_ ) return false;

    switch( e->type() )
    {
        case QEvent::Leave:
        case QEvent::FocusOut:
        case QEvent::WindowDeactivate:
        _setCursorState( CursorVisible );
        break;

        case QEvent::Enter:
        case QEvent::FocusIn:
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::Show:
        case QEvent::Hide:
        case QEvent::Wheel:
        _setCursorState( CursorVisible );
        if ( qobject_cast<QWidget*>( parent() )->hasFocus() )
        { autoHideTimer_.start( autoHideDelay_, this ); }
        break;

        default:
        break;
    }

    return false;
}

//_________________________________________________________
void CursorMonitor::timerEvent( QTimerEvent* e )
{

    if( e->timerId() == autoHideTimer_.timerId() )
    {

        autoHideTimer_.stop();
        _setCursorState( CursorHidden );

    } else QObject::timerEvent( e );

}

//_________________________________________________________
void CursorMonitor::_setCursorState( const CursorMonitor::CursorState& state )
{
    if( state == cursorState_ ) return;

    QWidget* widget( qobject_cast<QWidget*>( parent() ) );
    if( !widget ) return;

    switch( state )
    {
        case CursorVisible:
        {
            widget->setCursor( savedCursorShape_ );
            cursorState_ = CursorVisible;
            break;
        }

        case CursorHidden:
        {
            savedCursorShape_ = widget->cursor().shape();
            widget->setCursor( Qt::BlankCursor );
            cursorState_ = CursorHidden;
            break;
        }

        default: break;

    }

}

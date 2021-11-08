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

#include "CursorMonitor.h"
#include "XmlOptions.h"

//_________________________________________________________
CursorMonitor::CursorMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( QStringLiteral("CursorMonitor") ),
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
        _setCursorState( CursorState::Visible );
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
        _setCursorState( CursorState::Visible );
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

        _setCursorState( CursorState::Hidden );

    } else QObject::timerEvent( e );

}

//_________________________________________________________
void CursorMonitor::_setCursorState( CursorMonitor::CursorState state )
{

    // always stop timer
    autoHideTimer_.stop();

    // check if cursor state has changed
    if( state == cursorState_ ) return;

    // getp arent
    auto widget( qobject_cast<QWidget*>( parent() ) );
    if( !widget ) return;

    switch( state )
    {
        case CursorState::Visible:
        {
            widget->setCursor( savedCursorShape_ );
            cursorState_ = CursorState::Visible;
            break;
        }

        case CursorState::Hidden:
        {
            savedCursorShape_ = widget->cursor().shape();
            widget->setCursor( Qt::BlankCursor );
            cursorState_ = CursorState::Hidden;
            break;
        }

        default: break;

    }

}

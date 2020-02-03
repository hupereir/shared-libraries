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

#include "WidgetDragMonitor.h"
#include "XcbUtil.h"

#include <QMouseEvent>

//_________________________________________________________
WidgetDragMonitor::WidgetDragMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( "WidgetDragMonitor" ),
    mode_( ModeFlag::DragMove ),
    enabled_( false ),
    clickCounter_( this, 2 ),
    button_( Qt::NoButton ),
    target_( nullptr ),
    direction_( XcbDefines::Direction::_NET_WM_MOVERESIZE_MOVE ),
    isDragging_( false )
{ parent->installEventFilter(this); }

//_______________________________________________________
bool WidgetDragMonitor::eventFilter( QObject* object, QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress:
        {

            QWidget* widget( qobject_cast<QWidget*>( object ) );
            if( !widget ) return false;

            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
            button_ = mouseEvent->button();
            if( button_ == Qt::LeftButton )
            {

                event->accept();
                target_ = widget;
                isDragging_ = false;
                dragPosition_ = mouseEvent->pos();
                dragSize_ = widget->window()->size();

                // check modifiers
                if( ( mode_ & ModeFlag::DragResize ) && enabled_ && mouseEvent->modifiers() & Qt::ShiftModifier )
                {

                    direction_ = _direction( widget, mouseEvent->pos() );

                } else if( mode_ & ModeFlag::DragMove ) {

                    direction_ = XcbDefines::Direction::_NET_WM_MOVERESIZE_MOVE;
                    timer_.start( QApplication::doubleClickInterval(), this );

                    // handle multiple clicks
                    clickCounter_.increment();
                    if( clickCounter_.counts() == 2 )
                    {
                        emit stateChangeRequest();
                        timer_.stop();
                    }

                    return true;

                } else return false;

            }

            return false;

        }

        case QEvent::MouseButtonRelease:
        {
            event->accept();
            _resetDrag();
            return true;
        }

        case QEvent::MouseMove:
        {
            if( button_ != Qt::LeftButton ) return false;

            timer_.stop();

            // check against drag distance
            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
            if( QPoint( mouseEvent->pos() - dragPosition_ ).manhattanLength() < QApplication::startDragDistance() )
            { return false; }

            if( !(target_ && object == target_ ) ) return false;

            if( !enabled_ )
            {
                if( mode_ & ModeFlag::DragMove )
                {

                    emit stateChangeRequest();

                    event->accept();
                    return true;

                } else {

                    return false;

                }

            } else {

                if( (mode_ & ModeFlag::DragMove) && direction_ == XcbDefines::Direction::_NET_WM_MOVERESIZE_MOVE )
                {

                    // drag
                    if( !_startDrag() )
                    {
                        const QPoint position( target_->window() == target_  ? dragPosition_ : target_->mapTo( target_->window(), dragPosition_ ) );
                        target_->window()->move( mouseEvent->globalPos() - position );
                    }

                    event->accept();
                    return true;

                } else if( mode_ & ModeFlag::DragResize ) {

                    QRect newRect( target_->window()->geometry() );
                    const QPoint offset( mouseEvent->pos() - dragPosition_ );

                    switch( direction_ )
                    {

                        default:
                        case XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT:
                        newRect.setWidth( dragSize_.width() + offset.x() );
                        newRect.setHeight( dragSize_.height() + offset.y() );
                        break;

                        case XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT:
                        newRect.setLeft( mouseEvent->globalPos().x() );
                        newRect.setHeight( dragSize_.height() + offset.y() );
                        break;

                        case XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_TOPRIGHT:
                        newRect.setWidth( dragSize_.width() + offset.x() );
                        newRect.setTop( mouseEvent->globalPos().y() );
                        break;

                        case XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_TOPLEFT:
                        newRect.setLeft( mouseEvent->globalPos().x() );
                        newRect.setTop( mouseEvent->globalPos().y() );
                        break;
                    }

                    target_->window()->setGeometry( newRect );
                    event->accept();
                    return true;

                }

            }

            return false;
        }

        default: return false;

    }

}

//_______________________________________________________
void WidgetDragMonitor::timerEvent( QTimerEvent* event )
{
    if( event->timerId() != timer_.timerId() ) return QObject::timerEvent( event );

    timer_.stop();
    if( button_ != Qt::LeftButton ) return;

    if( !enabled_ ) emit stateChangeRequest();
    if( !enabled_ ) return;

    QMetaObject::invokeMethod( this, &WidgetDragMonitor::_startDrag, Qt::QueuedConnection );
}

//________________________________________________
XcbDefines::Direction WidgetDragMonitor::_direction( QWidget* widget, const QPoint& point ) const
{
    const int halfWidth( widget->width()/2 );
    const int halfHeight( widget->height()/2 );
    if( point.x() < halfWidth ) return point.y() < halfHeight ?  XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_TOPLEFT: XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT;
    else return point.y() < halfHeight ? XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_TOPRIGHT: XcbDefines::Direction::_NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT;
}

//___________________________________________________________
bool WidgetDragMonitor::_startDrag()
{

    if( !target_ ) return false;

    isDragging_ = true;

    #if WITH_XCB
    if( XcbUtil::get().moveWidget( target_->window(), target_->mapToGlobal( dragPosition_ ) ) )
    {

        _resetDrag();
        return true;

    }
    #endif

    return false;

}

//___________________________________________________________
void WidgetDragMonitor::_resetDrag()
{
    if( target_ ) target_->unsetCursor();
    target_ = nullptr;
    direction_ = XcbDefines::Direction::_NET_WM_MOVERESIZE_MOVE;
    button_ = Qt::NoButton;
    dragPosition_ = QPoint();
    timer_.stop();
    isDragging_ = false;
}

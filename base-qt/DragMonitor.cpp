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

#include "DragMonitor.h"

#include <QApplication>
#include <QMouseEvent>


//_____________________________________________________________
DragMonitor::DragMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( QStringLiteral("DragMonitor::DragMonitor") ),
    dragEnabled_( true ),
    dragInProgress_( false )
{
    parent->installEventFilter(this);
    setWindowGraggingEnabled(false);
}


//_____________________________________________________________
void DragMonitor::setWindowGraggingEnabled( bool value )
{ parent()->setProperty( "_kde_no_window_grab", !value ); }

//_____________________________________________________________
bool DragMonitor::eventFilter( QObject* target, QEvent* event )
{

    if( target != parent()  ) return false;
    switch( event->type() )
    {

        case QEvent::MouseButtonPress:
        {
            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>(event) );
            if( dragEnabled_ && mouseEvent->button() == Qt::LeftButton && mouseEvent->modifiers() == Qt::NoModifier )
            {
                dragInProgress_ = true;
                dragOrigin_ = mouseEvent->pos();
            }

            return false;

        }

        case QEvent::MouseMove:
        {
            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>(event) );
            if(  dragEnabled_ && ((mouseEvent->buttons() & Qt::LeftButton) && dragInProgress_ ) &&
                ( mouseEvent->pos() - dragOrigin_ ).manhattanLength() >= QApplication::startDragDistance() )
            {
                emit dragStarted( dragOrigin_ );
                dragInProgress_ = false;
            }

            return false;

        }

        case QEvent::MouseButtonRelease:
        {
           dragInProgress_ = false;
           return false;
        }

        default: return false;

    }


}

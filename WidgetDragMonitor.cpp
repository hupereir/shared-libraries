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

#include "WidgetDragMonitor.h"
#include "XmlOptions.h"

//_________________________________________________________
WidgetMonitor::WidgetMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( "WidgetDragMonitor" ),
{ parent->installEventFilter(this); }

//_______________________________________________________
bool WidgetMonitor::eventFilter( QObject* target, QEvent* event )
{

    if( target != parent()  ) return false;
    if( !_hasOptionName() ) return false;
    if(
        (event->type() == QEvent::Resize && ( mode_&Size )) ||
        (event->type() == QEvent::Move && ( mode_&Position ) ) )
    { timer_.start( 200, this ); }

    return false;
}

//_______________________________________________________
void WidgetMonitor::timerEvent( QTimerEvent* event )
{

    if( event->timerId() == timer_.timerId() )
    {

        // stop timer
        Q_ASSERT( _hasOptionName() );
        timer_.stop();

        // save size
        if( mode_&Size ) _saveWindowSize();
        if( mode_&Position ) _saveWindowPosition();

    } else return QObject::timerEvent( event );

}

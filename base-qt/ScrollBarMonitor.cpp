
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

#include "ScrollBarMonitor.h"

#include <QScrollBar>

//________________________________________________
ScrollBarMonitor::ScrollBarMonitor( QAbstractScrollArea* parent ):
    QObject( parent ),
    Counter( "ScrollBarMonitor" )
{}

//________________________________________________
void ScrollBarMonitor::save()
{
    auto scrollArea = static_cast<QAbstractScrollArea*>( parent() );
    if( scrollArea->verticalScrollBar() ) vertical_ = scrollArea->verticalScrollBar()->value();
    if( scrollArea->horizontalScrollBar() ) horizontal_ = scrollArea->horizontalScrollBar()->value();
}

//________________________________________________
void ScrollBarMonitor::restore() const
{
    auto scrollArea = static_cast<QAbstractScrollArea*>( parent() );
    if( scrollArea->verticalScrollBar() ) scrollArea->verticalScrollBar()->setValue( vertical_ );
    if( scrollArea->horizontalScrollBar() ) scrollArea->horizontalScrollBar()->setValue( horizontal_ );
}

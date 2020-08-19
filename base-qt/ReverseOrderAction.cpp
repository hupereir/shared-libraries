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

#include "ReverseOrderAction.h"
#include "Debug.h"
#include "TreeView.h"

#include <QHeaderView>

//_____________________________________________________
ReverseOrderAction::ReverseOrderAction( QWidget* parent, const QString& title ):
    QAction( parent ),
    Counter( QStringLiteral("ReverseOrderAction") )
{
    Debug::Throw( QStringLiteral("ReverseOrderAction::ReverseOrderAction.\n") );
    setText( title );
    connect( this, &QAction::triggered, this, &ReverseOrderAction::_reverseOrder );
}

//_____________________________________________________
ReverseOrderAction::ReverseOrderAction( QWidget* parent, QTreeView* target, const QString& title ):
    ReverseOrderAction( parent, title )
{ target_ = target; }

//_____________________________________________________
ReverseOrderAction::ReverseOrderAction( QWidget* parent, QHeaderView* header, const QString& title ):
    ReverseOrderAction( parent, title )
{ header_ = header; }

//______________________________________________________________________________
void ReverseOrderAction::_reverseOrder()
{
    Debug::Throw( QStringLiteral("ReverseOrderAction::_sort.\n") );
    
    // retrieve header
    if( target_ ) header_ = target_->header();
    if( !header_ ) return;
    
    // get sort order and invert
    auto order( header_->sortIndicatorOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder );
    header_->setSortIndicator( header_->sortIndicatorSection(), order );
}

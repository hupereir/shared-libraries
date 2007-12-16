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
  \file ItemDelegate.cpp
  \brief used to paint selected items with gradient
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>
#include <QModelIndex>

#include "ItemModel.h"
#include "ItemDelegate.h"

using namespace std;

//___________________________________________________
void ItemDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  
  Debug::Throw() << "ItemDelegate::paint - width: " << painter->device()->width() << endl;
  QStyleOptionViewItem new_option( option );
  
  //  highlight color
  QLinearGradient gradient(QPointF(0, 0), QPointF(painter->device()->width(), 0));
  QColor color( option.palette.color( QPalette::Highlight ) );
  gradient.setColorAt(0, color.light(130) );
  gradient.setColorAt(0.3, color );
  gradient.setColorAt(1, color.light(130) );
  new_option.palette.setBrush( QPalette::Highlight, QBrush( gradient ) );
  
  return QItemDelegate::paint( painter, new_option, index );
}

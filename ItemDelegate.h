#ifndef ItemDelegate_h
#define ItemDelegate_h

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
  \file ItemDelegate.h
  \brief used to paint selected items with gradient
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QItemDelegate>
#include "Counter.h"

//! custom delegate to repaint selection nicely
class ItemDelegate: public QItemDelegate, public Counter
{
  
  public: 
  
  //! constructor
  ItemDelegate( QObject* parent ):
    QItemDelegate( parent ),
    Counter( "ItemDelegate" )
  { Debug::Throw( "ItemDelegate::ItemDelegate.\n" ); }
        
  //! paint method
  virtual void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
      
};

#endif

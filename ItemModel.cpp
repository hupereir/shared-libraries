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
  \file    ItemModel.h
  \brief   Job model. Stores job information for display in lists
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "ItemModel.h"

using namespace std;

//_______________________________________________________________
ItemModel::ItemModel( QObject* parent ):
  QAbstractItemModel( parent ),
  Counter( "ItemModel" ),
  sort_column_(0),
  sort_order_( Qt::AscendingOrder )
{ Debug::Throw( "ItemModel::ItemModel.\n" ); }


//____________________________________________________________
void ItemModel::sort( int column, Qt::SortOrder order )
{ 
  Debug::Throw() << "ItemModel::sort - column: " << column << " order: " << order << endl;
  
  // store
  sort_column_ = column;
  sort_order_ = order;
    
}

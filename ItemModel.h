#ifndef ItemModel_h
#define ItemModel_h

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
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/


#include <QAbstractItemModel>

#include "Counter.h"

//! Job model. Stores job information for display in lists
class ItemModel : public QAbstractItemModel, public Counter
{
    
  public:
    
  //! constructor
  ItemModel(QObject *parent = 0);
  
  //! destructor
  virtual ~ItemModel()
  {}

  //! return all indexes in model starting from parent [recursive]
  QModelIndexList indexes( int column = 0, const QModelIndex& parent = QModelIndex() ) const;
  
  //!@name sorting
  //@{
  
  //! sort
  virtual void sort( void )
  { sort( sortColumn(), sortOrder() ); }
  
  //! sort
  virtual void sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
  
  //! current sorting column
  const int& sortColumn( void ) const
  { return sort_column_; }
  
  //! current sort order
  const Qt::SortOrder& sortOrder( void ) const
  { return sort_order_; }
  
  //@}
  
  protected:
  
  //! this sort columns without calling the layout changed callbacks
  virtual void _sort( void )
  { _sort( sortColumn(), sortOrder() ); }
  
  //! private sort, with no signals emmitted
  virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder ) = 0;
  
  
  private:  
  
  //! sorting column
  int sort_column_;
  
  //! sorting order
  Qt::SortOrder sort_order_;
  
};

#endif

#ifndef ListModel_h
#define ListModel_h

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
  \file    ListModel.h
  \brief   templatized list model
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <vector>

#include "Counter.h"
#include "ItemModel.h"

//! Job model. Stores job information for display in lists
template<class T> class ListModel : public ItemModel
{
   
  public:
      
  //! constructor
  ListModel(QObject *parent = 0):
    ItemModel( parent )
    {}
  
  //! destructor
  virtual ~ListModel()
  {}

  //!@name methods reimplemented from base class
  //@{
   
  //! flags
  virtual Qt::ItemFlags flags(const QModelIndex &index) const
  {
    if (!index.isValid()) return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
  
  //! unique index for given row, column and parent index
  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const
  {
  
    // check if index is valid
    if( !hasIndex( row, column, parent ) ) return QModelIndex();
    
    // return invalid index if parent is valid
    if( parent.isValid() ) return QModelIndex();
    
    // check against values_ 
    return ( row < (int) values_.size() ) ? createIndex( row, column ):QModelIndex();
  
  }
  
  //! index of parent
  virtual QModelIndex parent(const QModelIndex &index) const
  { return QModelIndex(); }
  
  //! number of rows below given index
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
  { return parent.isValid() ? 0:values_.size(); } 
  
  //@}
  
  //!@name interface
  //@{
  
  //! add value
  virtual void add( const T& value )
  { 
  
    Debug::Throw() << "ListModel::add" << std::endl;
    
    emit layoutAboutToBeChanged();
    
    typename T::List::iterator iter = std::find( values_.begin(), values_.end(), value );
    if( iter == values_.end() ) values_.push_back( value ); 
    else *iter = value;
    emit layoutChanged();
    
    // redo the sorting
    sort( sortColumn(), sortOrder() );
    
  }
  
  //! remove
  virtual void remove( const T& value )
  { 
    
    typename T::List::iterator iter = std::find( values_.begin(), values_.end(), value );
    if( iter == values_.end() ) return;
    emit layoutAboutToBeChanged();
    values_.erase( iter );
    emit layoutChanged();
    return;
    
  }
  
  //! update
  virtual void set( const typename T::List& values )
  { 
    emit layoutAboutToBeChanged();
    values_ = values;
    emit layoutChanged();
    sort( sortColumn(), sortOrder() );
    return;
  }
  
  //! return all values
  const typename T::List& get( void ) const
  { return values_; }
  
  //! return value for given index
  virtual T get( const QModelIndex& index ) const
  { return index.row() < values_.size() ? values_[index.row()]:T(); }
  
  //! return index associated to a given value
  virtual QModelIndex index( const T& value ) const
  { 
    typename T::List::const_iterator iter = std::find( values_.begin(), values_.end(), value );
    return (iter == values_.end()) ? QModelIndex():*iter;
  }

  //@}
       
  protected:
  
  //! return all values
  typename T::List& _get( void )
  { return values_; }
  
  private:
  
  //! values
  typename T::List values_;
 
};

#endif

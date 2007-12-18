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

  //!@name selection
  //@{
  
  //! clear internal list selected items
  virtual void clearSelectedIndexes( void )
  { selection_.clear(); }
  
  //! store index internal selection state
  virtual void setIndexSelected( const QModelIndex& index, bool value )
  { 
    if( value ) selection_.push_back( get(index) ); 
    else selection_.erase( std::remove( selection_.begin(), selection_.end(), get(index) ), selection_.end() );
  } 
      
  //! get list of internal selected items
  virtual QModelIndexList selectedIndexes( void ) const
  {
    
    QModelIndexList out;  
    for( typename T::List::const_iterator iter = selection_.begin(); iter != selection_.end(); iter++ )
    { out.push_back( index( *iter ) ); }
    return out;

  }
  
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
    
    emit layoutAboutToBeChanged();
    values_.erase( std::remove( values_.begin(), values_.end(), value ), values_.end() );
    selection_.erase( std::remove( selection_.begin(), selection_.end(), value ), selection_.end() );
    emit layoutChanged();
    return;
    
  }
  
  //! reset
  virtual void reset( void )
  { set( typename T::List() ); }
  
  //! update
  virtual void set( const typename T::List& values )
  { 
    
    emit layoutAboutToBeChanged();
    values_ = values;
    selection_.clear();
    emit layoutChanged();
    
    sort( sortColumn(), sortOrder() );
    
    return;
  }
  
  //! return all values
  const typename T::List& get( void ) const
  { return values_; }
  
  //! return value for given index
  virtual T get( const QModelIndex& index ) const
  { return (index.isValid() && index.row() < values_.size() ) ? values_[index.row()]:T(); }
  
  //! return all values
  typename T::List get( const QModelIndexList& list ) const
  { 
    typename T::List out;
    for( QModelIndexList::const_iterator iter = list.begin(); iter != list.end(); iter++ )
    { if( iter->isValid() && iter->row() < values_.size() ) out.push_back( get( *iter ) ); }
    return out;
  }
  
  //! return index associated to a given value
  virtual QModelIndex index( const T& value ) const
  { 
    for( unsigned int row=0; row<values_.size(); row++ )
    { if( value == values_[row] ) return index( row, 0 ); }
    return QModelIndex();
  }

  //@}
       
  protected:
  
  //! return all values
  typename T::List& _get( void )
  { return values_; }
  
  private:
  
  //! values
  typename T::List values_;
  
  //! selection
  typename T::List selection_;
 
};

#endif

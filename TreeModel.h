#ifndef TreeModel_h
#define TreeModel_h

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
  \file    TreeModel.h
  \brief   Job model. Stores job information for display in lists
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <list>

#include "Counter.h"
#include "ItemModel.h"
#include "TreeItem.h"

//! Job model. Stores job information for display in lists
template<class T> class TreeModel : public ItemModel
{
    
  public:
     
  //! value type
  typedef T ValueType;
  
  //! pointer
  typedef T* Pointer;
  
  //! list of vector
  typedef std::vector<ValueType> List;  

  //! item
  typedef TreeItem<T> Item;
  
  //! constructor
  TreeModel(QObject *parent = 0):
    ItemModel( parent ),
    map_( typename Item::Map() ),
    root_( map_ )
  {}
  
  //! destructor
  virtual ~TreeModel()
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
  
    // fid parent item
    const Item& parent_item = parent.isValid() ? _find( parent.internalId() ) : root_;
    
    // return new index if matching child is found, or invalid index
    return (row < (int)parent_item.childCount()) ? 
      createIndex( row, column, parent_item.child(row).id() ):
      QModelIndex();
    
  }

  
  //! index of parent
  virtual QModelIndex parent(const QModelIndex &index) const
  {

    // check index validity
    if (!index.isValid()) return QModelIndex();

    // retrieve associated job item
    const Item& child_item( _find(index.internalId() ) );

    // if no parent return invalid index
    if( !( child_item.hasParent() && child_item.parent().hasParent() ) ) return QModelIndex();
  
    // if parent is root return invalid index
    const Item& parent_item( child_item.parent() );
    const Item& grand_parent_item( parent_item.parent() );
  
    // find parent position in list of grand parent
    int row(0);
    for( ; row < int( grand_parent_item.childCount() ); row++ )
    { if( &parent_item == &grand_parent_item.child(row) ) return createIndex( row, 0, parent_item.id() ); }
  
    // not found
    return QModelIndex();
  
  }
  
  //! number of rows below given index
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const
  {
  
    // check column
    if (parent.column() > 0) return 0;

    // store parent Job
    const Item& parent_item = parent.isValid() ? _find( parent.internalId() ) : root_;
    return parent_item.childCount();  
    
  } 

  //@}
  
  //!@name job to index matching
  //@{
  
  //! return index associated to a given value, starting from parent [recursive]
  virtual QModelIndex index( const ValueType& value, const QModelIndex& parent = QModelIndex() ) const
  {

    // return parent index if job match
    if( parent.isValid() && _find( parent.internalId() ).get() == value ) return parent;
      
    // get rows associated to parent and loop over rows  
    for( int row=0; row<rowCount( parent ); row++ )
    {
      // get child index
      QModelIndex found( index( value, index( row, 0, parent ) ) );
      if( found.isValid() ) return found;
    }
 
    // value not found. return invalid index
    return QModelIndex();
  
  }
   
  //! return all values [recursive]
  virtual List children( const QModelIndex& parent = QModelIndex() ) const
  {

    List out;    
    
    // retrieve matching item
    // loop over parent children
    for( int row = 0; row < rowCount( parent ); row++ )
    {
      
      // create child index
      QModelIndex index( TreeModel::index( row, 0, parent ) );
      if( !index.isValid() ) continue;
      
      // retrieve matching item
      const Item& item( _find( index.internalId() ) );
      out.push_back( item.get() );
      
      // retrieve child children, and insert
      List children( TreeModel::children( index ) );
      out.insert( out.end(), children.begin(), children.end() );
      
    }
    
    return out;
      
  }
  
  //! return value associated to given model index
  virtual ValueType get( const QModelIndex& index ) const
  {
    if( !index.isValid() ) return ValueType();
    else return _find( index.internalId() ).get();
  }
  
  //! return all values matching index list
  List get( const QModelIndexList& indexes ) const
  { 
    List out;
    for( QModelIndexList::const_iterator iter = indexes.begin(); iter != indexes.end(); iter++ )
    { if( iter->isValid() ) out.push_back( get( *iter ) ); }
    return out;
  }

  //@}

  //!@name selection
  //@{
  
  //! clear internal list selected items
  void clearSelectedIndexes( void )
  {
    for( typename Item::Map::iterator iter = map_.begin(); iter != map_.end(); iter++ )
    { iter->second->setFlag( Item::SELECTED, false ); }
  }  
  
  //! store index internal selection state
  void setIndexSelected( const QModelIndex& index, bool value )
  {
    if( !index.isValid() ) return;
    typename Item::Map::iterator iter( map_.find( index.internalId() ) );
    if( iter != map_.end() ) iter->second->setFlag( Item::SELECTED, value );
  }  
  
  //! get list of internal selected items
  QModelIndexList selectedIndexes( void ) const
  {
    QModelIndexList out;
    for( typename Item::Map::const_iterator iter = map_.begin(); iter != map_.end(); iter++ )
    {
      
      if( !iter->second->flag( Item::SELECTED ) ) continue;
      
      // retrieve and check index associated to job
      QModelIndex index( TreeModel::index( iter->second->get() ) );
      if( index.isValid() ) out.push_back( index );
      
    }
    
    return out;
  }    
  
  //@}

  //!@name expansion
  //@{
  
  //! clear internal list of expanded items
  void clearExpandedIndexes( void )
  {
    for( typename Item::Map::iterator iter = map_.begin(); iter != map_.end(); iter++ )
    { iter->second->setFlag( Item::EXPANDED, false ); }
  }    
  
  //! store index internal selection state
  void setIndexExpanded( const QModelIndex& index, bool value ) 
  {
    if( !index.isValid() ) return;
    typename Item::Map::iterator iter( map_.find( index.internalId() ) );
    if( iter != map_.end() ) iter->second->setFlag( Item::EXPANDED, value );
  }  
  
  //! get list of internal selected items
  QModelIndexList expandedIndexes( void ) const
  {
    QModelIndexList out;
    for( typename Item::Map::const_iterator iter = map_.begin(); iter != map_.end(); iter++ )
    {
      
      if( !iter->second->flag( Item::EXPANDED ) ) continue;
      
      // retrieve and check index associated to job
      QModelIndex index( TreeModel::index( iter->second->get() ) );
      if( index.isValid() ) out.push_back( index );
      
    }
    
    return out;
  }  
 
  //@}
  
  //! clear
  void clear( void )
  {
    
    emit layoutAboutToBeChanged();
    map_.clear();
    root_ = Item( map_ );
    emit layoutChanged();
   
  }
  
  //! update values
  void add( const ValueType& value )
  { add( List(1,value) ); }
    
  //! update values
  void add( const List& values )
  {  
  
    // prepare modifications
    emit layoutAboutToBeChanged();
    List local( values );
   _add( local );  
  
    // modifications done
    emit layoutChanged();
    
    return;
    
  }
     
  //! update values
  void set( const List& values )
  {  
  
    // prepare modifications
    emit layoutAboutToBeChanged();

    // first update the TreeItem structure
    // then redraw tree
    List local( values );
    _update( root_, local );
    _add( local );  
  
    // modifications done
    emit layoutChanged();
    
    return;
    
  }
    
  //! root item
  const Item& root( void ) const
  { return root_; }

  protected:

  //! update (recusive)
  static void _update( Item& parent, List& values )
  {
              
    // first update children that are found in set
    for( unsigned int row = 0; row < parent.childCount(); )
    { 
      typename List::iterator found( std::find( values.begin(), values.end(), parent.child(row).get() ) );
      if( found == values.end() ) parent.remove( row );
      else {
        parent.child(row).set( *found );
        values.erase( found );
        row++;
      }
    }
    
    // update (recursive) children with remaining values
    for( unsigned int row = 0; row < parent.childCount(); row++ )
    { _update( parent.child(row), values ); }  
  
  }

  //! add
  void _add( List& values )
  {
  
    for( typename List::iterator iter = values.begin(); iter != values.end(); iter++ )
    { root_.add( *iter ); }
  
  }
  
  //! find item matching id
  const Item& _find( typename Item::Id id ) const
  {
    typename Item::Map::const_iterator iter( map_.find( id ) );
    return iter == map_.end() ? root_:*iter->second;
  }
  
  private:
  
  //! item map
  /*! used to allow fast mapping between index and value */
  typename Item::Map map_;
  
  //! root item
  Item root_;
  
};

#endif

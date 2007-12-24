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
class TreeModel : public ItemModel
{
    
  public:
     
  //! value type
  typedef T ValueType;
  
  //! reference
  typedef T& Reference;

  //! pointer
  typedef T* Pointer;
  
  //! list of vector
  typedef std::vector<ValueType> List;  

  //! item
  typedef TreeItem<T> Item;
  
  //! constructor
  TreeModel(QObject *parent = 0):
    ItemModel( parent ),
    map_( Item::Map() ),
    root_( map_ )
  
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
    for( ; row < grand_parent_item.childCount(); row++ )
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
  
  //! return index associated to a given job is, starting from parent [recursive]
  virtual QModelIndex index( const Reference value, const QModelIndex& parent = QModelIndex() ) const
  {

    // return parent index if job match
    if( parent.isValid() && parent.get() == value ) return parent;
      
    // get rows associated to parent and loop over rows  
    for( int row=0; row<rowCount( parent ); row++ )
    {
      // get child index
      QModelIndex found( index( value, index( row, 0, parent ) ) );
      if( found.isValid() ) return found;
    }
 
    // job not found. return invalid index
    return QModelIndex();
  
  }
   
  //! return Job associated to given model
  virtual ValueType get( const QModelIndex& index ) const
  {
    if( !index.isValid() ) return ValueType();
    else return _find( index.internalId() ).get();
  }
  
  //@}
    
  // //! root item
  // const Item& root( void ) const
  // { return root_; }
  
  //! update jobs
  void set( const List& values )
  {  
  
    // prepare modifications
    emit layoutAboutToBeChanged();

    // store jobs
    values_ = values;
  
    // first update the TreeItem structure
    // then redraw tree
    List local( values_ );
    _update( root_, local );
    _add( local );  
  
    // modifications done
    emit layoutChanged();
    
    return;
    
  }

    
  //! clear tree
  /*! stored values are not changed but they are all as top level */
  void clearTree( void )
  {

    // copy all jobs to local set, set parent id to 0
    for( List::iterator iter = values_.begin(); iter != values_.end(); iter++ )
    {
      Job job( *iter );
      job.setParentId( 0 );
      local.insert( job );
    }
  
    // reassign
    setJobs( local );
    
    return; 
  }    
      
  protected:

  //! update (recusive)
  static void _update( Item& parent, List& values );
  
  //! add
  void _add( List& values );
  
  //! find item matching id
  const Item& _find( Item::Id value ) const;
  
  private:
  
  //! item map
  /*! used to allow fast mapping between index and value */
  Item::Map map_;
  
  //! root item
  Item root_;
  
  //! current set of values
  List values_;
 
};

#endif

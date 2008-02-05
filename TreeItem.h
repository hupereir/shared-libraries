#ifndef TreeItem_h
#define TreeItem_h

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
  \file    TreeItem.h
  \brief   used to wrap object T into tree structure
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <assert.h>
#include <map>
#include <vector>

#include "TreeItemBase.h"

//! used to wrap object T into tree structure
template<class T> class TreeItem: public TreeItemBase
{
  
  public:
  
  //! value type
  typedef T ValueType;
  
  //! reference
  typedef T& Reference;

  //! pointer
  typedef T* Pointer;

  //! list of vector
  typedef std::vector<TreeItem> List;  

  //! list of vector
  typedef std::map<int, TreeItem*> Map;  

  //! id type
  typedef unsigned int Id;
  
  //! root constructor
  TreeItem( Map& item_map ):
    TreeItemBase(0),
    map_( item_map ),
    parent_(0)
  { map_[id()] = this; }
  
  //! copy constructor
  TreeItem( const TreeItem& item ):
    TreeItemBase( item.id() ),
    map_( item.map_ ),
    parent_( item.parent_ ),
    value_( item.value_ ),
    children_( item.children_ )
  {
    
    // flags
    setFlags( item.flags() );
    
    // store id in map
    map_[id()] = this;
    
    // reassign parents
    for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
    { iter->parent_ = this; } 
    
  }
  
  //! assignment
  TreeItem& operator = ( const TreeItem& item )
  {

    // flags
    setFlags( item.flags() );
    
    parent_ = item.parent_;
    value_ = item.value_;
    children_ = item.children_;

    // erase current id from map    
    // update and store in map
    _eraseFromMap();
    _setId( item.id() );
    map_[id()] = this;

    // reassign parents
    for( typename List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
    { iter->parent_ = this; } 
  
    return *this;  
  }

  //! destructor
  virtual ~TreeItem( void )
  { _eraseFromMap(); }
 
  //! less than operator
  bool operator < (const TreeItem& item ) const
  { return get() < item.get(); }

  //! equal to operator
  bool operator == (const TreeItem& item ) const
  { return get() == item.get(); }

  //! clear children
  void clear( void )
  { children_.clear(); }
 
  //! value
  const ValueType& get( void ) const
  { return value_; }

  //! value
  void set( const Reference value )
  { value_ = value; }
    
  //! parent
  bool hasParent( void ) const
  { return parent_; }
  
  //! parent
  const TreeItem& parent( void ) const
  { 
    assert( parent_ );
    return *parent_; 
  }
  
  //! child count
  unsigned int childCount( void ) const
  { return children_.size(); }
  
  //! get child at given row
  TreeItem& child( unsigned int row ) 
  { 
    assert( row < childCount() );
    return children_[row]; 
  }
  
  //! children
  const TreeItem& child( unsigned int row ) const
  { 
    assert( row < childCount() );
    return children_[row]; 
  }
  
  //! remove child at given row
  bool remove( unsigned int row )
  {
    if( row >= childCount() ) return false;
    unsigned int local(0);
    typename List::iterator iter = children_.begin();
    for( ; iter != children_.end() && local < row; iter++, local++ ) {}
    children_.erase( iter );
    return true;
  }
  
  //! add child [recursive]
  bool add( const Reference value )
  {

    // try add to this
    if( value.isChild( get() ) )
    {
      // loop over children, see if one match
      bool found( false );
      for( typename List::iterator iter = children_.begin(); iter != children_.end() && !found; iter++ )
      {
        if( iter->get() == value ) 
        {
          iter->set( value );
          found = true;
        }
      }
      
      // add if not found
      if( !found ) children_.push_back( TreeItem( map_, this, value ) );
      
      // sort
      _sort();
      return true;
      
    }
    
    // try add to children
    bool added(false);
    for( typename List::iterator iter = children_.begin(); iter != children_.end() && !added; iter++ )
    { added = iter->add( value ); }
    
    // add to this if top level
    if( !( added || hasParent() ) )
    { 
      children_.push_back( TreeItem( map_, this, value ) );
      _sort();
      return true;
    }
    
    return added;
  }

  protected:

  //! constructor
  /*! used to insert T in the tree structure */
  TreeItem( Map& item_map, const TreeItem* parent, const Reference value ):
    TreeItemBase( ++_runningId() ),
    map_( item_map ),
    parent_( parent ),
    value_( value )
  { map_[id()] = this; }
    
  //! value
  Reference _get( void )
  { return value_; }

  //! value
  void _set( const Reference value )
  { value_ = value; }

  //! erase from map
  void _eraseFromMap( void )
  {
    typename Map::iterator iter( map_.find( id() ) );
    if( iter != map_.end() && iter->second == this ) map_.erase( id() );
    
  }

  //! sorting
  void _sort()
  { 
    if( children_.empty() ) return;
    std::sort( children_.begin(), children_.end() ); 
  }
  
  private:
  
  //! item map
  Map& map_;

  //! parent
  const TreeItem* parent_;
  
  //! associated value
  ValueType value_;
  
  //! list of children
  List children_;

  //! streamer
  friend std::ostream& operator << (std::ostream& out , const TreeItem& item )
  {

    // indent 2 space characters per parent
    const TreeItem* parent( item.parent_ );
    while( parent )
    {
      out << "  ";
      parent = parent->parent_;
    }
  
    // print job pid
    out << item.id() << " " << item.get() << std::endl;
  
    // print job children
    for( unsigned int i=0; i<item.childCount(); i++ )
    { out << item.child(i); }    
    
    return out;
    
  }
  
};

#endif

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

#include <vector>
#include <map>


//! used to wrap object T into tree structure
template<class T> class TreeItem
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

  //! list of vector
  typedef std::map<int, TreeItem*> Map;  

  //! id type
  typedef unsigned int Id;
  
  //! root constructor
  TreeItem( Map& item_map ):
    map_( item_map ),
    parent_(0)
  { 
    running_id_ = id_ = 0;
    map_[id()] = this; 
  }
  
  //! copy constructor
  TreeItem( const TreeItem& item ):
    map_( item.map_ ),
    parent_( item.parent_ ),
    value_( item.value_ ),
    children_( item.children_ ),
    id_( item.id() )
  {
    // store id in map
    map_[id()] = this;
    
    // reassign parents
    for( List::iterator iter = children_.begin(); iter != children_.end(); iter++ )
    { iter->_setParent( this ); } 
    
  }
  
  //! assignment
  TreeItem& operator = ( const TreeItem& item )
  {
    parent_ = item.parent_;
    value_ = item.value_;
    children_ = item.children_;

    // erase current id from map
    if( map_[id()] == this ) map_.erase( id() );
    
    // update id and store in map
    id_ = item.id();
    map_[id()] = this;

    // reassign parents
    for( ChildList::iterator iter = children_.begin(); iter != children_.end(); iter++ )
    { iter->_setParent( this ); } 
  
    return *this;  
  }

  //! destructor
  virtual ~TreeItem( void )
  { if( map_[id()] == this ) map_.erase( id() ); }
 
  //! clear children
  void clear( void )
  { children_.clear(); }

  //! id
  const unsigned int& id( void ) const
  { return id_; }
  
  //! value
  const Reference get( void ) const
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
  TreeItem child( unsigned int row ) 
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
    bool found( false );
    unsigned int local(0);
    ChildList::iterator iter = children_.begin();
    for( ; iter != children_.end() && local < row; iter++, local++ ) {}
    children_.erase( iter );
    return true;
  }
  
  //! add child [recursive]
  bool add( const Reference value )
  {

    // try add to this
    if( get() == value.parent() )
    {
      children_.push_back( TreeItem( map_, this, value ) );
      return true;
    }
    
    // try add to children
    bool added(false);
    for( ChildList::iterator iter = children_.begin(); iter != children_.end() && !added; iter++ )
    { added = iter->add( job ); }
    
    // add to this if top level
    if( !( added || hasParent() ) )
    { 
      children_.push_back( TreeItem( map_, this, value ) );
      return true;
    }
    
    return added;
  }

  protected:

  //! constructor
  TreeItem( Map& item_map, const TreeItem* parent, const Reference value ):
    map_( item_map ),
    parent_( parent ),
    value_( value ),
    id_( running_id_++ )
  { map_[id()] = this }
    
  //! value
  Reference _get( void )
  { return value_; }

  //! value
  void _set( const Reference value )
  { value_ = value; }

  private:
  
  //! item map
  Map& map_;

  //! parent
  TreeItem* parent;
  
  //! associated value
  ValueType value_;
  
  //! list of children
  List children_;
  
  //! item unique id
  Id id_;
  
  //! running id
  static Id running_id_;
  
};

#endif

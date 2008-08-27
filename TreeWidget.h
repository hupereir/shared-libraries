#ifndef _TreeWidget_h_
#define _TreeWidget_h_

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
  \file TreeWidget.h
  \brief qlistview with even objects highlighted with different color
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QDomDocument>
#include <QDomElement>
#include <QMenu>
#include <QTreeWidget>

#include <string>
#include <vector>

#include "Counter.h"
#include "Debug.h"

//! qlistview with even objects highlighted with different color
class TreeWidget: public QTreeWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! constructor
  TreeWidget( QWidget* parent );
     
  //! destructor
  ~TreeWidget( void )
  {}

  //!@name contextual menu
  //@{
  
  //! returns true if menu has been initialized
  virtual bool hasMenu( void ) const
  { return menu_; }
  
  //! retrieve popup menu
  virtual QMenu& menu( void );
  
  //! set icon size manually and disable option
  void setIconSize( const QSize& size )
  {
    QTreeWidget::setIconSize( size );
    icon_size_from_options_ = false;
  }
  
  //@} 
  
  //! set column name
  virtual void setColumnName( const int& column, const std::string& name );
  
  //! enumeration for column type (for sorting)
  enum ColumnType {
  
    //! used to tell if a column should be considered as a string for sorting
    STRING,
    
    //! used to tell if a column should be considered as a number for sorting
    NUMBER 
    
  };
  
  //! column type
  virtual void setColumnType( const int& column, const ColumnType& type )
  { 
    assert( column >=0 );
    if( static_cast<int>(column_types_.size()) <= column ) column_types_.resize( column+1, STRING );
    column_types_[column] = type; 
  }
  
  //! column type
  virtual ColumnType columnType( const int& column ) const
  { 
    assert( column >=0 );
    return ( column < static_cast<int>(column_types_.size()) ) ? column_types_[column]:STRING; 
  }
  
  //! column type
  virtual void clearColumnTypes( void )
  { column_types_.clear(); }
         
  //! number of visible columns
  virtual int visibleColumnCount( void ) const;
  
  
  //!@name column mask
  //@{
  
  //! option name
  virtual void setMaskOptionName( const std::string& value );
  
  //! option name 
  virtual bool hasMaskOptionName( void ) const
  { return !maskOptionName().empty(); }
  
  //! option name
  virtual const std::string& maskOptionName( void ) const
  { return mask_option_name_; }
  
  //! get mask
  virtual unsigned int mask( void ) const;

  //! set mask manually
  virtual void setMask( const unsigned int& );
  
  //! update column mask from option, if any
  virtual void updateMask( void );
  
  //! save column mask to option, if any
  virtual void saveMask( void );
   
  //@}
  
  //! delete items and all subitems recursively
  static void deleteItemRecursive( QTreeWidgetItem* item );
 
  //! html
  QDomElement htmlElement( QDomDocument& document );
   
  //! selected items
  template <typename T> 
  QList< T* > selectedItems( void )
  {
    QList<QTreeWidgetItem*> found( QTreeWidget::selectedItems() );
    QList<T*> out;
    for( QList<QTreeWidgetItem*>::const_iterator iter = found.begin(); iter!=found.end(); iter++ )
    {
      T* local_item( static_cast<T*>( *iter ) );
      if( local_item ) out.push_back( local_item );
    }
    return out;
  }

  //! current items
  template <typename T> 
  T* currentItem( void )
  { return static_cast<T*>( QTreeWidget::currentItem() ); }

  //! retrieve all children, starting from parent, recursively
  QList<QTreeWidgetItem*> children( QTreeWidgetItem* parent = 0 );
 
  //! retrieve all children, starting from parent, recursively
  template <typename T> 
  QList< T* > children( QTreeWidgetItem* parent = 0 )
  {
    Debug::Throw( "TreeWidget::children (templates).\n" );
    QList<QTreeWidgetItem*> children( TreeWidget::children( parent ) );
    QList<T*> out;
    for( QList<QTreeWidgetItem*>::const_iterator iter = children.begin(); iter != children.end(); iter++ )
    {
      T* local_item( dynamic_cast<T*>( *iter ) );
      if( local_item ) out.push_back( local_item );
    }
    return out;
  }

  //! list view items to implement sorting based on column type
  class Item: public QTreeWidgetItem, public Counter
  {
    
    public:
    
    //! constructor
    Item():
      Counter( "TreeWidget::Item" )
    {}
    
    //! destructor
    virtual ~Item( void )
    {}
    
    //! color
    void setColor( const QColor& color )
    { color_ = color; }
    
    //! retrieve color
    const QColor& color( void )
    { return color_; }
    
    //! flag selection
    void setFlag( const Qt::ItemFlag& flag, const bool &value )
    { 
      if( value ) setFlags( flags() | flag );
      else setFlags( flags() & ~Qt::ItemFlags(flag) );
    }
        
    //! order operator
    virtual bool operator<( const QTreeWidgetItem &other ) const;
    
    protected:
    
    //! alternate color for item painting
    QColor color_;
    
  };
  
  public slots:
  
  //! sort items (based on current column )
  virtual void sort( void );
   
  
  //! resize all visible columns to match contents
  virtual void resizeColumns()
  { resizeColumns( mask() ); }
   
  //! resize all visible columns to match contents
  virtual void resizeColumns( const unsigned int& mask );

  protected:
   
  //! paint event
  virtual void paintEvent( QPaintEvent* );
  
  //! selected column color
  const QColor& _selectedColumnColor( void ) const
  { return selected_column_color_; }

  //! selected column color
  void _setSelectedColumnColor( const QColor& color )
  { selected_column_color_ = color; }
  
  protected slots:

  //! popup contextual menu
  virtual void _raiseMenu( const QPoint& );
 
  //! header menu
  virtual void _raiseHeaderMenu( const QPoint& ); 
  
  private slots:
  
  //! update alternate item color
  void _updateConfiguration( void );

  private:

  //! popup menu for right click
  QMenu *menu_;
  
  //! icon size is picked from options
  bool icon_size_from_options_;
  
  //! mask option name
  std::string mask_option_name_;
  
  //! column types
  std::vector<ColumnType> column_types_;
  
  //! selected column background color
  QColor selected_column_color_;

};

#endif

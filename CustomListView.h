#ifndef _CustomListView_h_
#define _CustomListView_h_

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
  \file CustomListView.h
  \brief qlistview with even objects highlighted with different color
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QDomDocument>
#include <QDomElement>
#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>

#include <string>
#include <vector>

#include "Counter.h"
#include "Debug.h"

//! qlistview with even objects highlighted with different color
class CustomListView: public QTreeWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! constructor
  CustomListView( QWidget* parent );
     
  //! destructor
  ~CustomListView( void )
  {}

  //! returns true if menu has been initialized
  virtual bool hasMenu( void ) const
  { return menu_; }
  
  //! retrieve popup menu
  virtual QMenu& menu( void )
  {  
    if( !hasMenu() ) menu_ = new QMenu( this );
    return *menu_;
  }
    
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
    Exception::assert( column >=0, DESCRIPTION( "invalid column" ) );
    if( static_cast<int>(column_types_.size()) <= column ) column_types_.resize( column+1, STRING );
    column_types_[column] = type; 
  }
  
  //! column type
  virtual ColumnType columnType( const int& column ) const
  { 
    Exception::assert( column >=0, DESCRIPTION( "invalid column" ) );
    return ( column < static_cast<int>(column_types_.size()) ) ? column_types_[column]:STRING; 
  }
  
  //! column type
  virtual void clearColumnTypes( void )
  { column_types_.clear(); }
  
  //! add context menu action
  virtual QAction& addMenuAction( const std::string& name, const bool& need_selection = false )
  {
    QAction* out = menu().addAction( name.c_str() );
    if( need_selection ) selection_actions_.push_back( out );
    return *out;
  }
  
  //! add context menu action
  virtual QAction& addMenuAction( const std::string& name, QObject* reciever, const std::string& slot,  const bool& need_selection = false )
  {
    QAction& out( addMenuAction( name, need_selection ) );
    connect( &out, SIGNAL( triggered() ), reciever, slot.c_str() );
    return out;
  }
  
  //! clear "selected" actions
  void clearSelectedActions( void )
  { selection_actions_.clear(); }
     
  //! return column visibility bitset. Is 1 for shown columns, 0 for hidden
  virtual unsigned int mask( void );
  
  //! show/hide columns according to mask bitset. 1 is for shown columns, 0 for hidden
  virtual void setMask( const unsigned int& mask );
  
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
  
  //! retrieve all items
  QList<QTreeWidgetItem*> items( void );
 
  //! retrieve all items
  template <typename T> 
  QList< T* > items( void )
  {
    QList<QTreeWidgetItem*> children( items() );
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
    Item( CustomListView* parent ):
      QTreeWidgetItem( parent ),
      Counter( "CustomListView::Item" )
    {}
    
    //! constructor
    Item( Item* item ):
      QTreeWidgetItem( item ),
      Counter( "CustomListView::Item" )
    {}
    
    //! destructor
    virtual ~Item( void )
    {}
    
    //! flag selection
    void setFlag( const Qt::ItemFlag& flag, const bool &value )
    { 
      if( value ) setFlags( flags() | flag );
      else setFlags( flags() ^ ~Qt::ItemFlags(flag) );
    }
    
    //! order operator
    virtual bool operator<( const QTreeWidgetItem &other ) const;
    
  };
  
  public slots:
  
  //! sort items (based on current column )
  virtual void sort( void );
  
  //! sort items by column (no order is provided)
  virtual void sortItems( int column )
  { if( column != sortColumn() ) sortByColumn( column ); }
  
  //! update alternate item color
  virtual void updateItemColor( void );
  
  protected:
  
  //! mouse button press event handler. Pops up attached menu.
  virtual void mousePressEvent( QMouseEvent *e );
          
  private:

  //! popup menu for right click
  QMenu *menu_;
  
  //! column mask
  /*! gets reinitialized anytime GetMask is called */
  unsigned int mask_;    

  //! column types
  std::vector<ColumnType> column_types_;
   
  //! actions that are enabled/disabled depending on selection
  std::vector<QAction*> selection_actions_;

};

#endif

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

#include <set>
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
      
  //! returns true if menu has been initialized
  bool hasMenu( void ) const
  { return menu_; }
  
  //! retrieve popup menu
  QMenu& menu( void )
  {  
    if( !hasMenu() ) menu_ = new QMenu( this );
    return *menu_;
  }
  
  // set column name
  void setColumnName( const int& column, const std::string& name );
  
//   //! add a menu item
//   int AddMenuItem( const std::string& name, QObject* reciever, const std::string& slot, const bool& need_selection = false );
// 
//   //! add a menu item
//   int AddMenuItem( const std::string& name, QMenu* reciever, const bool& need_selection = false );  
   
  //! return column visibility bitset. Is 1 for shown columns, 0 for hidden
  unsigned int mask( void );
  
  //! show/hide columns according to mask bitset. 1 is for shown columns, 0 for hidden
  void setMask( const unsigned int& mask );
  
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

  public slots:
  
  //! update alternate item color
  void updateItemColor( void );
  
  protected:
  
  //! mouse button press event handler. Pops up attached menu.
  void mousePressEvent( QMouseEvent *e );
          
  private:

  //! popup menu for right click
  QMenu *menu_;
  
  //! column mask
  /*! gets reinitialized anytime GetMask is called */
  unsigned int mask_;    
   
  //! menu items that must be enabled/disabled depending on selection
  std::set<int> selection_menu_items_;

};

#endif

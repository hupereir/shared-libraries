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
  \file CustomListView.cc
  \brief qlistview for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QCursor>
#include <QMouseEvent>

#include "CustomListView.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
CustomListView::CustomListView( QWidget* parent ):
  QTreeWidget( parent ),
  Counter( "CustomListView" ),
  menu_( 0 )
{
  Debug::Throw( "CustomListView::CustomListView" );   
  updateItemColor();
  
  connect( qApp, SIGNAL( configurationChanged() ), this, SLOT( updateItemColor() ) );
}

//__________________________________________________________________________
void CustomListView::setColumnName( const int& column, const string& name )
{
  Debug::Throw( "CustomListView::setColumnName.\n" );
  Exception::assert( column>=0 && column <columnCount(), DESCRIPTION( "invalid index" ) );
    
  // retrieve HeaderItem
  QTreeWidgetItem* item( headerItem() );
  if( !item ) { setHeaderItem( item = new QTreeWidgetItem( this ) ); }

  item->setText( column, name.c_str() );
  
}

//_______________________________________________
unsigned int CustomListView::mask( void )
{
  mask_ = 0;
  for( int index=0; index < columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask_ |= (1<<index);
  return mask_;
}

//______________________________________________________
void CustomListView::setMask( const unsigned int& mask )
{
  bool modified( false );
  for( int index=0; index < columnCount(); index++ )
  {
    // see if there is a change between new and old mask
    if( mask_ & mask & (1<<index) ) continue;
    bool visible( mask & (1<<index) );
    setColumnHidden( index, !(mask & (1<<index) ) );
  }
  
}
 
//_______________________________________________________
void CustomListView::deleteItemRecursive( QTreeWidgetItem* item )
{
  
  Debug::Throw( "CustomListView::deleteItemRecursive.\n" );
  
  // retrieve all children
  QList< QTreeWidgetItem* > children( item->takeChildren() );
  for( QList<QTreeWidgetItem*>::iterator child_iter = children.begin(); child_iter != children.end(); child_iter++ )
  { delete *child_iter; }
  delete item;
  
}

//___________________________________
QDomElement CustomListView::htmlElement( QDomDocument& document )
{
 
  Debug::Throw( "CustomListView::htmlElement.\n" );
  
  QDomElement out = document.createElement( "table" );
  out.setAttribute( "border", "1" );
  
  // retrieve mask
  unsigned int mask( this->mask() );
  
  // column names
  QDomElement row = out.appendChild( document.createElement( "tr" ) ).toElement();
  for( int i = 0; i<columnCount(); i++ )
  {
    
    // skip hidden columns
    if( !( mask&(1<<i) ) ) continue;
    row.
      appendChild( document.createElement( "td" ) ).
      appendChild( document.createTextNode( headerItem()->text(i) ) );
  }
  
  // retrieve items
  QList< QTreeWidgetItem* > children( items() );
  for( QList< QTreeWidgetItem* >::iterator iter = children.begin(); iter != children.end(); iter++ )
  {
    QDomElement row = out.appendChild( document.createElement( "tr" ) ).toElement();
    for( int i = 0; i<columnCount(); i++ )
    {
      
      // skip hidden columns
      if( !( mask&(1<<i) ) ) continue;
      row.
        appendChild( document.createElement( "td" ) ).
        appendChild( document.createTextNode( (*iter)->text(i) ) );
    }
  }
  
  return out;
  
}

//___________________________________
void CustomListView::mousePressEvent( QMouseEvent *event )
{
  Debug::Throw( "CustomListView::mousePressEvent.\n" );
  if( event->button() != Qt::RightButton ) {
    QTreeWidget::mousePressEvent( event );
    return;
  }
  
  if( !hasMenu() ) return;
    
  // enable/disable
  bool has_selection( !QTreeWidget::selectedItems().empty() );
  for( vector<QAction*>::iterator iter = selection_actions_.begin(); iter != selection_actions_.end(); iter++ )
  { (*iter)->setEnabled( has_selection ); }
  
  // move and show menu
  menu().adjustSize();
  QtUtil::moveWidget( &menu(), QCursor::pos() );
  menu().show();
  
}

//___________________________________
QList< QTreeWidgetItem* > CustomListView::items( void )
{
  
  Debug::Throw( "CustomListView::items.\n" );
  
  QList<QTreeWidgetItem* > out;
  for( int i = 0; i<topLevelItemCount(); i++ )
  {
    QTreeWidgetItem* item( topLevelItem( i ) );
    
    // add top level item
    out.push_back( item );
    
    // add children
    for( int j=0; j<item->childCount(); j++ )
    { out.push_back( item->child( i ) ); }
  
  }
  
  return out;
}

//___________________________________
bool CustomListView::Item::operator < (const QTreeWidgetItem& item ) const
{

  // cast parent to custom list view
  const CustomListView* parent( dynamic_cast<const CustomListView*>( treeWidget() ) );
  if( !parent ) return QTreeWidgetItem::operator < (item);
  
  // retrieve column type
  int column( parent->sortColumn() );
  CustomListView::ColumnType column_type( parent->columnType( column ) );
  
  switch( column_type )
  {
    
    case STRING:
    return text( column ) < item.text( column );
    break;
  
    case NUMBER:
    return Str( qPrintable( text( column ) ) ).get<double>() < Str( qPrintable( item.text( column ) ) ).get<double>();
    break;
  
    default:
    throw runtime_error( DESCRIPTION( "unsupported column type" ) );
    break;
    
  }
  
  // useless
  return QTreeWidgetItem::operator < (item);
  
}

//_____________________________________________________________________
void CustomListView::sort( void )
{
  Debug::Throw( "CustomListView::sort.\n" );
  if( !isSortingEnabled() ) return;

  setUpdatesEnabled( false );
  sortByColumn( sortColumn() );
  sortByColumn( sortColumn() );
  setUpdatesEnabled( true );
  repaint();
  
}

//_____________________________________________________________________
void CustomListView::updateItemColor( void )
{
  Debug::Throw( "CustomListView::updateItemColor.\n" );
  
  if( !XmlOptions::get().find( "ITEM_COLOR" ) )
  {
    setAlternatingRowColors( false ); 
    return;
  }
  
  QColor item_color( XmlOptions::get().get<string>("ITEM_COLOR").c_str() );
  if( !item_color.isValid() )
  {
    setAlternatingRowColors( false ); 
    return;
  }
  
  QPalette palette( this->palette() );
  palette.setColor( QPalette::AlternateBase, item_color );
  setPalette( palette );
  setAlternatingRowColors( true ); 
}

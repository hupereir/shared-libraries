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
  \file TreeWidget.cpp
  \brief qlistview for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QCursor>
#include <QHeaderView>
#include <QPainter>

#include "ColorDisplay.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "TreeWidget.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
TreeWidget::TreeWidget( QWidget* parent ):
  QTreeWidget( parent ),
  Counter( "TreeWidget" ),
  menu_( 0 )
{
  Debug::Throw( "TreeWidget::TreeWidget.\n" );   

  // default configuration
  setRootIsDecorated( false );
  setSortingEnabled( true );
  
  // header menu
  header()->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raiseHeaderMenu( const QPoint& ) ) );
  
  // configuration
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
  
}

//_______________________________________________
QMenu& TreeWidget::menu( void )
{  
  if( !hasMenu() ) 
  {
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raiseMenu( const QPoint& ) ) );
    menu_ = new QMenu( this );
  }
  
  return *menu_;

}

//__________________________________________________________________________
void TreeWidget::setColumnName( const int& column, const string& name )
{
  
  Debug::Throw( "TreeWidget::setColumnName.\n" );
  assert( column>=0 && column <columnCount() );
    
  // retrieve HeaderItem
  QTreeWidgetItem* item( headerItem() );
  if( !item ) { setHeaderItem( item = new QTreeWidgetItem( this ) ); }

  item->setText( column, name.c_str() );
  
}

//_______________________________________________
int TreeWidget::visibleColumnCount( void ) const
{
  int out(0);
  for( int index=0; index < columnCount(); index++ )
  if( !isColumnHidden( index ) ) out++;
  return out;
}


//_______________________________________________
void TreeWidget::setMaskOptionName( const std::string& value )
{ 
  Debug::Throw( "TreeWidget::setMaskOptionName.\n" );
  mask_option_name_ = value; 
  if( !XmlOptions::get().find( maskOptionName() ) ) saveMask();
}

//_______________________________________________
unsigned int TreeWidget::mask( void ) const
{
  Debug::Throw( "TreeWidget::mask.\n" );
  unsigned int mask = 0;
  assert( model() );
  for( int index=0; model() && index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask |= (1<<index);
  return mask;
}

//_______________________________________________
void TreeWidget::setMask( const unsigned int& mask )
{
  
  Debug::Throw( "TreeWidget::setMask.\n" );
  for( int index=0; index < model()->columnCount(); index++ )
  {
    
    // see if there is a change between new and old mask
    if( isColumnHidden( index ) == !(mask & (1<<index) ) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
    
  }  
  
  return;
  
}

//_______________________________________________
void TreeWidget::updateMask( void )
{

  Debug::Throw( "TreeWidget::updateMask.\n" );

  // check model and option availability
  if( !model() ) return;
  if( !hasMaskOptionName() ) return;
  if( !XmlOptions::get().find( maskOptionName() ) ) return;
  
  // assign mask from options
  setMask( XmlOptions::get().get<unsigned int>( maskOptionName() ) );
  resizeColumns();
    
}

//_______________________________________________
void TreeWidget::saveMask( void )
{
  
  Debug::Throw( "TreeWidget::saveMask.\n" );
  if( !hasMaskOptionName() ) return;
  XmlOptions::get().set<unsigned int>( maskOptionName(), mask() );

}

//_______________________________________________________
void TreeWidget::deleteItemRecursive( QTreeWidgetItem* item )
{
  
  Debug::Throw( "TreeWidget::deleteItemRecursive.\n" );
  
  // retrieve all children
  QList< QTreeWidgetItem* > children( item->takeChildren() );
  for( QList<QTreeWidgetItem*>::iterator child_iter = children.begin(); child_iter != children.end(); child_iter++ )
  { delete *child_iter; }
  delete item;
  
}

//___________________________________
QDomElement TreeWidget::htmlElement( QDomDocument& document )
{
 
  Debug::Throw( "TreeWidget::htmlElement.\n" );
  
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
  QList< QTreeWidgetItem* > children( TreeWidget::children() );
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
QList< QTreeWidgetItem* > TreeWidget::children( QTreeWidgetItem* parent )
{
  
  Debug::Throw( 2, "TreeWidget::children.\n" );
  
  QList<QTreeWidgetItem* > out;
  
  if( parent == 0 )
  {
    
    for( int i = 0; i<topLevelItemCount(); i++ )
    {
      out.push_back( topLevelItem( i ) );
      out += children( topLevelItem( i ) );
    }
      
  } else {
    
    for( int i=0; i<parent->childCount(); i++ ) 
    {
      out.push_back( parent->child(i) );
      out += children( parent->child(i) );
    }
    
  }
  
  return out;
  
}

//___________________________________
bool TreeWidget::Item::operator < (const QTreeWidgetItem& item ) const
{

  // cast parent to custom list view
  const TreeWidget* parent( dynamic_cast<const TreeWidget*>( treeWidget() ) );
  if( !parent ) return QTreeWidgetItem::operator < (item);
  
  // retrieve column type
  int column( parent->sortColumn() );
  TreeWidget::ColumnType column_type( parent->columnType( column ) );
  
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
void TreeWidget::sort( void )
{
  Debug::Throw( "TreeWidget::sort.\n" );
  if( !isSortingEnabled() ) return;
  
  sortItems( sortColumn(), header()->sortIndicatorOrder() );
  repaint();
  
}


//______________________________________________________
void TreeWidget::resizeColumns( const unsigned int& mask )
{

  // if no items present, do nothing
  if( !( model() && model()->rowCount() ) ) return;
  
  for( int i = 0; i < model()->columnCount(); i++ )
  { if( mask & (1<<i) ) resizeColumnToContents(i); }
  
}

//__________________________________________________________
void TreeWidget::paintEvent( QPaintEvent* event )
{

  // check selected column background color
  if( !_selectedColumnColor().isValid() ) return QTreeWidget::paintEvent( event );
  
  // check number of columns
  if( visibleColumnCount() < 2 ) return QTreeWidget::paintEvent( event );
 
  // get selected column
  int selected_column( sortColumn() );
  QRect rect( visualRect( model()->index( 0, selected_column ) ) );
  
  QPainter painter( viewport() );
  rect.setTop(0);
  rect.setHeight( height() );
  painter.setBrush( _selectedColumnColor() );
  painter.setPen( Qt::NoPen );
  painter.drawRect( rect.intersected( event->rect() ) );
  painter.end();
  
  return QTreeWidget::paintEvent( event );

}

//___________________________________
void TreeWidget::_raiseMenu( const QPoint & pos )
{ 
  
  Debug::Throw( "TreeWidget::_raiseMenu.\n" );
  
  // check if menu was created
  if( !hasMenu() ) return;
      
  // move and show menu
  menu().adjustSize();
  QtUtil::moveWidget( &menu(), QCursor::pos() );
  menu().show();
  
}

//___________________________________
void TreeWidget::_raiseHeaderMenu( const QPoint & pos )
{ 
  Debug::Throw( "TreeWidget::_raiseHeaderMenu.\n" ); 
  
  // check number of columns
  if( header()->count() <= 1 ) return;
  
  // create menu and raise.
  ColumnSelectionMenu menu( this, this );
  if( isSortingEnabled() ) 
  {
    menu.addSeparator();
    menu.addMenu( new ColumnSortingMenu( &menu, this ) );
  }

  menu.adjustSize();
  menu.exec( QCursor::pos() );
  
  // save mask to keep sync with column visibility
  saveMask();
  
}

//_____________________________________________________________________
void TreeWidget::_updateConfiguration( void )
{
   
  Debug::Throw( "TreeWidget::_updateConfiguration.\n" );
 
  // load mask from option, if any
  updateMask();
  
  // try load alternate colors from option
  QColor color;
  Str colorname( XmlOptions::get().get<string>("ALTERNATE_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) color = QColor( colorname.c_str() );
  
  if( !color.isValid() ) { setAlternatingRowColors( false ); }
  else {
    QPalette palette( this->palette() );
    palette.setColor( QPalette::AlternateBase, color );
    setPalette( palette );
    setAlternatingRowColors( true ); 
  }
  
  // try load selected column color from option
  color = QColor();
  colorname = Str( XmlOptions::get().get<string>("SELECTED_COLUMN_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) color = QColor( colorname.c_str() );
  _setSelectedColumnColor( color );
    
  // icon size
  int icon_size( XmlOptions::get().get<int>( "LIST_ICON_SIZE" ) );
  setIconSize( QSize( icon_size, icon_size )  );
  
}

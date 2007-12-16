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
  Exception::check( column>=0 && column <columnCount(), DESCRIPTION( "invalid index" ) );
    
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
unsigned int TreeWidget::mask( void )
{
  mask_ = 0;
  for( int index=0; index < columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask_ |= (1<<index);
  return mask_;
}

//______________________________________________________
void TreeWidget::setMask( const unsigned int& mask )
{
  for( int index=0; index < columnCount(); index++ )
  {
    // see if there is a change between new and old mask
    if( mask_ & mask & (1<<index) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
  }  
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
  painter.drawRect( rect );
  
  return QTreeWidget::paintEvent( event );

}

//__________________________________________________________
void TreeWidget::drawRow( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  
  Item* item = dynamic_cast<Item*>(itemFromIndex( index ) );
  
  // modify options and pass to the default method
  QStyleOptionViewItem new_option( option );
  
  //  QGradient 
  QLinearGradient gradient(QPointF(0, 0), QPointF(width(), 0));
  
  if( item && item->color().isValid() )
  {
    
    if( isItemSelected( item ) ) 
    {
      
      gradient.setColorAt(0, item->color().light(130));
      gradient.setColorAt(0.3, item->color());
      gradient.setColorAt(1, item->color().light(130));
      new_option.palette.setBrush( QPalette::Highlight, QBrush( gradient ) );
      
    } else new_option.palette.setColor( QPalette::Text, item->color() );
      
  } else if( isItemSelected( item ) ) {
    
    QColor color( option.palette.color( QPalette::Highlight ) );
    gradient.setColorAt(0, color.light(130) );
    gradient.setColorAt(0.3, color );
    gradient.setColorAt(1, color.light(130) );
    new_option.palette.setBrush( QPalette::Highlight, QBrush( gradient ) );
  
  }
  
  return QTreeWidget::drawRow( painter, new_option, index );

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

//_____________________________________________________________________
void TreeWidget::_updateConfiguration( void )
{
   
  Debug::Throw( "TreeWidget::_updateConfiguration.\n" );
  
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
  
}

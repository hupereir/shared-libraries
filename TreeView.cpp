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
  \file TreeView.cpp
  \brief customized Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QCursor>
#include <QHeaderView>
#include <QPainter>

#include "ColorDisplay.h"
#include "QtUtil.h"
#include "TreeView.h"
#include "ItemModel.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;

//_______________________________________________
TreeView::TreeView( QWidget* parent ):
  QTreeView( parent ),
  Counter( "TreeView" ),
  menu_( 0 ),
  mask_(0)
{
  Debug::Throw( "TreeView::TreeView.\n" );   
    
  // actions
  _installActions();

  // default configuration
  setRootIsDecorated( false );
  setSortingEnabled( true );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
  
}

//_______________________________________________
QMenu& TreeView::menu( void )
{  
  if( !hasMenu() ) 
  {
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raiseMenu( const QPoint& ) ) );
    menu_ = new QMenu( this );
  }
  
  return *menu_;

}

//_______________________________________________
bool TreeView::isVisible( const QModelIndex& index ) const
{
  if( !index.isValid() ) return false;
  QModelIndex parent( model()->parent( index ) );
  return (!parent.isValid() ) || ( isExpanded( parent ) && isVisible( parent ) );
}
  
//_______________________________________________
int TreeView::visibleColumnCount( void ) const
{
  int out(0);
  for( int index=0; index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) out++;
  return out;
}

//_______________________________________________
unsigned int TreeView::mask( void )
{
  mask_ = 0;
  for( int index=0; index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask_ |= (1<<index);
  return mask_;
}

//______________________________________________________
void TreeView::setMask( const unsigned int& mask )
{
  for( int index=0; index < model()->columnCount(); index++ )
  {
    // see if there is a change between new and old mask
    if( mask_ & mask & (1<<index) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
  }  
}  

//__________________________________________________________
void TreeView::paintEvent( QPaintEvent* event )
{
 
  // check selected column background color
  if( !_selectedColumnColor().isValid() ) return QTreeView::paintEvent( event );
  
  // check number of columns
  if( visibleColumnCount() < 2 ) return QTreeView::paintEvent( event );
  
  // try cast model
  ItemModel* model( dynamic_cast<ItemModel*>( TreeView::model() ) ); 
  if( !model ) return QTreeView::paintEvent( event );
  
  // get selected column
  int selected_column( model->sortColumn() );
  QRect rect( visualRect( model->index( 0, selected_column ) ) );
  
  QPainter painter( viewport() );
  rect.setTop(0);
  rect.setHeight( height() );
  painter.setBrush( _selectedColumnColor() );
  painter.setPen( Qt::NoPen );
  painter.drawRect( rect.intersected( event->rect() ) );
  
  return QTreeView::paintEvent( event );
  
}

//__________________________________________________________
void TreeView::drawRow( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{    
  // modify options and pass to the default method
  QStyleOptionViewItem new_option( option );
  
  //  QGradient 
  QLinearGradient gradient(QPointF(0, 0), QPointF(width(), 0));    
  QColor color( option.palette.color( QPalette::Highlight ) );
  gradient.setColorAt(0, color.light(130) );
  gradient.setColorAt(0.3, color );
  gradient.setColorAt(1, color.light(130) );
  new_option.palette.setBrush( QPalette::Highlight, QBrush( gradient ) );
  
  return QTreeView::drawRow( painter, new_option, index );

}

//__________________________________________________________
void TreeView::_installActions( void )
{
  Debug::Throw( "TreeView::_installActions.\n" );

  addAction( select_all_action_ = new QAction( "Select all", this ) );
  select_all_action_->setShortcut( CTRL+Key_A );
  select_all_action_->setShortcutContext( WidgetShortcut );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );

}

//___________________________________
void TreeView::_raiseMenu( const QPoint & pos )
{ 
  
  Debug::Throw( "TreeView::_raiseMenu.\n" );
  
  // check if menu was created
  if( !hasMenu() ) return;
      
  // move and show menu
  menu().adjustSize();
  QtUtil::moveWidget( &menu(), QCursor::pos() );
  menu().show();
  
}

//_____________________________________________________________________
void TreeView::_updateConfiguration( void )
{
  Debug::Throw( "TreeView::_updateConfiguration.\n" );
    
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

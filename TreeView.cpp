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

#include <assert.h>
#include <QApplication>
#include <QCursor>
#include <QHeaderView>
#include <QPainter>

#include "ColorDisplay.h"
#include "ColumnSelectionMenu.h"
#include "ColumnSortingMenu.h"
#include "QtUtil.h"
#include "TreeView.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;

//_______________________________________________
TreeView::TreeView( QWidget* parent ):
  QTreeView( parent ),
  Counter( "TreeView" ),
  menu_( 0 ),
  icon_size_from_options_( true )
{
  Debug::Throw( "TreeView::TreeView.\n" );   
    
  // actions
  _installActions();

  // default configuration
  setAllColumnsShowFocus( true );
  setRootIsDecorated( false );
  setSortingEnabled( true );
  setExpandsOnDoubleClick( false );
  
  // header menu
  header()->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( header(), SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _raiseHeaderMenu( const QPoint& ) ) );

  // configuration
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
  if( !( model() && index.isValid() ) ) return false;
  QModelIndex parent( model()->parent( index ) );
  return (!parent.isValid() ) || ( isExpanded( parent ) && isVisible( parent ) );
}

//_______________________________________________
int TreeView::visibleColumnCount( void ) const
{
  int out(0);
  for( int index=0; model() && index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) out++;
  return out;
}

//_______________________________________________
void TreeView::setMaskOptionName( const std::string& value )
{ 

  Debug::Throw( "TreeView::setMaskOptionName.\n" );
  mask_option_name_ = value; 
  if( !XmlOptions::get().find( maskOptionName() ) ) saveMask();

}

//_______________________________________________
unsigned int TreeView::mask( void ) const
{
  Debug::Throw( "TreeView::mask.\n" );
  assert( model() );
  unsigned int mask = 0;
  for( int index=0; model() && index < model()->columnCount(); index++ )
  if( !isColumnHidden( index ) ) mask |= (1<<index);
  return mask;
}

//_______________________________________________
void TreeView::setMask( const unsigned int& mask )
{
  
  Debug::Throw( "TreeView::setMask.\n" );
  for( int index=0; index < model()->columnCount(); index++ )
  {
    
    // see if there is a change between new and old mask
    if( isColumnHidden( index ) == !(mask & (1<<index) ) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
    
  }  
  
  return;
  
}

//______________________________________________________
void TreeView::resizeColumns( const unsigned int& mask )
{

  // if no items present, do nothing
  if( !( model() && model()->rowCount() ) ) return;
  
  for( int i = 0; i < model()->columnCount(); i++ )
  { if( mask & (1<<i) ) resizeColumnToContents(i); }
  
}

//_______________________________________________
void TreeView::updateMask( void )
{

  Debug::Throw( "TreeView::updateMask.\n" );

  // check model and option availability
  if( !model() ) return;
  if( !hasMaskOptionName() ) return;
  if( !XmlOptions::get().find( maskOptionName() ) ) return;
  
  // assign mask from options
  setMask( XmlOptions::get().get<unsigned int>( maskOptionName() ) );
  resizeColumns();
    
}

//_______________________________________________
void TreeView::saveMask( void )
{
  
  Debug::Throw( "TreeView::saveMask.\n" );
  if( !hasMaskOptionName() ) return;
  XmlOptions::get().set<unsigned int>( maskOptionName(), mask() );

}

//__________________________________________________________
void TreeView::paintEvent( QPaintEvent* event )
{
 
  // check selected column background color
  if( !_selectedColumnColor().isValid() ) return QTreeView::paintEvent( event );
  
  // check number of columns
  if( visibleColumnCount() < 2 ) return QTreeView::paintEvent( event );
  if( header() && header()->isVisible() && header()->isSortIndicatorShown() )
  {

    // get selected column
    int selected_column( header()->sortIndicatorSection() );
    QRect rect( visualRect( model()->index( 0, selected_column ) ) );
    
    QPainter painter( viewport() );
    rect.setTop(0);
    rect.setHeight( height() );
    painter.setBrush( _selectedColumnColor() );
    painter.setPen( Qt::NoPen );
    painter.drawRect( rect.intersected( event->rect() ) );
    painter.end();
    
  }
  
  return QTreeView::paintEvent( event );
  
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

//___________________________________
void TreeView::_raiseHeaderMenu( const QPoint & pos )
{ 
  Debug::Throw( "TreeView::_raiseHeaderMenu.\n" ); 
  
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
  
  // save mask after menu execution, 
  // to keep visible columns in sync with option
  saveMask();
  
}

//_____________________________________________________________________
void TreeView::_updateConfiguration( void )
{
  Debug::Throw( "TreeView::_updateConfiguration.\n" );
    
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
  if( icon_size_from_options_ )
  {
    int icon_size( XmlOptions::get().get<int>( "LIST_ICON_SIZE" ) );
    QTreeView::setIconSize( QSize( icon_size, icon_size )  );
  } 
    
}

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
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
TreeView::TreeView( QWidget* parent ):
  QTreeView( parent ),
  Counter( "TreeView" ),
  menu_( 0 ),
  columns_(0),
  mask_(0)
{
  Debug::Throw( "TreeView::TreeView.\n" );   

  // default configuration
  setRootIsDecorated( false );
  setSortingEnabled( true );

  updateItemColor();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateItemColor() ) );
  
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
unsigned int TreeView::mask( void )
{
  mask_ = 0;
  for( int index=0; index < columns_; index++ )
  if( !isColumnHidden( index ) ) mask_ |= (1<<index);
  return mask_;
}

//______________________________________________________
void TreeView::setMask( const unsigned int& mask )
{
  for( int index=0; index < columns_; index++ )
  {
    // see if there is a change between new and old mask
    if( mask_ & mask & (1<<index) ) continue;
    setColumnHidden( index, !(mask & (1<<index) ) );
  }  
}  

//_____________________________________________________________________
void TreeView::updateItemColor( void )
{
  Debug::Throw( "TreeView::updateItemColor.\n" );
    
  // try load from option
  QColor item_color;
  Str colorname( XmlOptions::get().get<string>("ITEM_COLOR").c_str() );
  if( !colorname.isEqual( qPrintable( ColorDisplay::NONE ), false ) ) item_color = QColor( colorname.c_str() );
  
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

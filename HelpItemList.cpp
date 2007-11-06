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
   \file    HelpItemList.cpp
   \brief   list to display help item entries
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QApplication>
#include <QMouseEvent>

#include "HelpItemList.h"

using namespace std;
using namespace BASE;

//_______________________________________________
const QString HelpItemList::DRAG = "HelpItemList::Drag";

//__________________________________________________________
HelpItemList::HelpItemList( QWidget* parent ):
  CustomListBox( parent ),
  drag_enabled_( false )
{
  Debug::Throw( "HelpItemList::HelpItemList.\n" );
  setAcceptDrops(true);  
  setAutoScroll(true);
}
    
//__________________________________________________________
void HelpItemList::mousePressEvent( QMouseEvent* event )
{
  
  Debug::Throw( "HelpItemList::mousePressEvent.\n" );
  
  // left button handling
  if (event->button() == Qt::LeftButton) 
  {
    
    // store event position for drag-start
    drag_start_ = event->pos();
        
  }
  
  return CustomListBox::mousePressEvent( event );

}
  
//_____________________________________________________________
void HelpItemList::mouseMoveEvent( QMouseEvent* event )
{
  
  Debug::Throw(0, "HelpItemList::mouseMoveEvent.\n" );
  
  // check button
  if( !(event->buttons()&Qt::LeftButton) )  return CustomListBox::mouseMoveEvent( event );
  
  // check distance to last click
  if( (event->pos() - drag_start_ ).manhattanLength() < QApplication::startDragDistance() )
  { return CustomListBox::mouseMoveEvent( event ); }
  
  _startDrag( event );
  
}

//__________________________________________________________
void HelpItemList::dragEnterEvent( QDragEnterEvent* event )
{
  Debug::Throw(0, "HelpItemList::dragEnterEvent.\n" );

  // check if object can be decoded
  if( !_acceptDrag( event ) ) 
  {
    Debug::Throw(0, "HelpItemList::dragEnterEvent - rejected.\n" ); 
    event->ignore();
    return;
  }
  
  // accept event
  event->acceptProposedAction() ;

  // retrieve item below pointer
  Item *item( static_cast<Item*>( itemAt( event->pos()) ) );

  if( item ) {
        
    drop_item_ = item;    
    drop_item_selected_ = isItemSelected( drop_item_ );
    setItemSelected( drop_item_, true );
    
  }
      
}

//__________________________________________________________
void HelpItemList::dragMoveEvent( QDragMoveEvent* event )
{
  Debug::Throw(0, "HelpItemList::dragMoveEvent.\n" ); 

  // check if object can be decoded
  if( !_acceptDrag( event ) ) 
  {
    Debug::Throw(0, "HelpItemList::dragMoveEvent - rejected.\n" ); 
    event->ignore();
    return;
  }
  
  // accept event
  event->acceptProposedAction() ;
  
  // retrieve item below pointer
  Item *item( static_cast<Item*>( itemAt( event->pos() ) ) );
  
  // update drop item  
  if( item ) 
  {            
        
    if( drop_item_ != item )
    {

      if( drop_item_ ) setItemSelected( drop_item_, drop_item_selected_ );
      
      // change drop item
      drop_item_ = item;
      
      drop_item_selected_ = isItemSelected( drop_item_ );
      if( drop_item_ ) setItemSelected( drop_item_, true );
      
    }
    
  }
  
}

//__________________________________________________________
void HelpItemList::dropEvent( QDropEvent* event )
{
  Debug::Throw( "HelpItemList::dropEvent.\n" );

  // check if object can be decoded
  if( !_acceptDrag( event ) ) event->ignore();
  else
  {
    event->acceptProposedAction();
    _processDrop( event );
    _resetDrag();
  }
  
  return;
  
}

//__________________________________________________________
bool HelpItemList::_acceptDrag( QDropEvent *event ) const
{
  
  Debug::Throw( "HelpItemList::_acceptDrag.\n" );
  return drag_enabled_ && event->mimeData()->hasFormat( HelpItemList::DRAG );
   
}

//__________________________________________________________
bool HelpItemList::_startDrag( QMouseEvent *event )
{
  
  Debug::Throw( 0, "HelpItemList::_startDrag.\n" );

  // start drag
  QDrag *drag = new QDrag(this);
  
  // store data
  QMimeData *mime = new QMimeData();
  mime->setData( HelpItemList::DRAG, QListWidget::currentItem()->text().toAscii() );
  mime->setText( QListWidget::currentItem()->text() );
  drag->setMimeData(mime);
  drag->start(Qt::CopyAction);  
  return true;
  
}

//__________________________________________________________
void HelpItemList::_resetDrag( void )
{
  
  Debug::Throw( "HelpItemList::_resetDrag.\n" );

  // cleans drop_item
  if( drop_item_ ) setItemSelected( drop_item_, drop_item_selected_ );
  drop_item_ = 0;
  
}

//__________________________________________________________
bool HelpItemList::_processDrop( QDropEvent *event )
{
  
  Debug::Throw( "HelpItemList::_processDrop.\n" );
  
//   // retrieve item below pointer
//   Item *item( static_cast<Item*>(itemAt( event->pos() ) ) );
//   if( !item ) return false;
//   
//   // retrieve event keyword
//   const QMimeData& mime( *event->mimeData() );
//   if( mime.hasFormat( HelpItemList::DRAG ) )
//   {
//    
//     // dragging from one keyword to another
//     // retrieve old keyword
//     QString value( mime.data( HelpItemList::DRAG ) );
//     if( value.isNull() || value.isEmpty() ) return false;
//     
//     _resetDrag();
//     
//     // retrieve full keyword of selected items
//     QList<Item*> items( selectedItems<Item>() );
//     for( QList<Item*>::iterator iter = items.begin(); iter != items.end(); iter++ )
//     {
//       string old_keyword( keyword( *iter ) );
//       
//       // make sure that one does not move an item to itself
//       if( old_keyword == current_keyword ) continue;
// 
//       emit keywordChanged( old_keyword, current_keyword + "/" + qPrintable( value ) );
//     }
//     
//     return true;
//   }
//   
//   if( mime.hasFormat( LogEntryList::DRAG ) )
//   {
//     
//     // dragging from logEntry list
//     _resetDrag();
//     emit entryKeywordChanged( current_keyword );
//     
//   }
//   
//   return false;
  return true;
}

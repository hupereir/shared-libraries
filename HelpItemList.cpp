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
#include <QHeaderView>

#include "HelpItemList.h"

using namespace std;
using namespace BASE;

//_______________________________________________
const QString HelpItemList::DRAG = "HelpItemList::Drag";

//__________________________________________________________
HelpItemList::HelpItemList( QWidget* parent ):
  TreeWidget( parent ),
  drag_enabled_( false )
{
  Debug::Throw( "HelpItemList::HelpItemList.\n" );
  setAcceptDrops(true);  
  setAutoScroll(true);
  setSortingEnabled( false );
  
  setColumnCount( 1 );
  setHeaderItem(0);
  setColumnName( 0, "" );
  
  header()->hide();
  
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
  
  return TreeWidget::mousePressEvent( event );

}
  
//_____________________________________________________________
void HelpItemList::mouseMoveEvent( QMouseEvent* event )
{
  
  Debug::Throw( "HelpItemList::mouseMoveEvent.\n" );
  
  // check button
  if( !(event->buttons()&Qt::LeftButton) ) return TreeWidget::mouseMoveEvent( event );
  
  // check if drag enabled
  if( !drag_enabled_ ) return TreeWidget::mouseMoveEvent( event );
  
  // check distance to last click
  if( (event->pos() - drag_start_ ).manhattanLength() < QApplication::startDragDistance() )
  { return TreeWidget::mouseMoveEvent( event ); }
 
  _startDrag( event );
  
}

//__________________________________________________________
void HelpItemList::dragEnterEvent( QDragEnterEvent* event )
{
  Debug::Throw( "HelpItemList::dragEnterEvent.\n" );

  // check if object can be decoded
  if( !_acceptDrag( event ) ) 
  {
    Debug::Throw( "HelpItemList::dragEnterEvent - rejected.\n" ); 
    event->ignore();
    return;
  }
  
  // accept event
  event->acceptProposedAction() ;

  // retrieve item below pointer
  Item *item( static_cast<Item*>( itemAt( event->pos()) ) );
  if( item ) {
    
    drag_item_ = item;
    setCurrentItem( item );
    
  }
      
}

//__________________________________________________________
void HelpItemList::dragMoveEvent( QDragMoveEvent* event )
{
  Debug::Throw( "HelpItemList::dragMoveEvent.\n" ); 

  // check if object can be decoded
  if( !_acceptDrag( event ) ) 
  {
    Debug::Throw( "HelpItemList::dragMoveEvent - rejected.\n" ); 
    event->ignore();
    return;
  }
  
  // accept event
  event->acceptProposedAction() ;
   
  // retrieve item below pointer
  Item *item( static_cast<Item*>( itemAt( event->pos() ) ) );
  
  // update drop item  
  if( !(item && drag_item_ ) ) return;
   
  int first_index = indexOfTopLevelItem( drag_item_ );
  int last_index = indexOfTopLevelItem( item );
  item = static_cast<Item*>( takeTopLevelItem( first_index ) );
  insertTopLevelItem( last_index, item );
  drag_item_ = item;
  
  setCurrentItem( item );
  
}

//__________________________________________________________
void HelpItemList::dropEvent( QDropEvent* event )
{
  Debug::Throw( "HelpItemList::dropEvent.\n" );

  if( !_acceptDrag( event ) ) event->ignore();
  else emit itemMoved();
  
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
  
  Debug::Throw( "HelpItemList::_startDrag.\n" );

  // start drag
  QDrag *drag = new QDrag(this);
  
  // store data
  QMimeData *mime = new QMimeData();
  mime->setData( HelpItemList::DRAG, "" );
  mime->setText( QTreeWidget::currentItem()->text(0) );
  drag->setMimeData(mime);
  drag->start(Qt::CopyAction);
    
  return true;
  
}

//__________________________________________________________
void HelpItemList::_resetDrag( void )
{
  
  Debug::Throw( "HelpItemList::_resetDrag.\n" );
  drag_item_ = 0;
  
}

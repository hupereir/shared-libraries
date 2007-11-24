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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file BlockHighlight.cpp
  \brief handles multiple clicks and timeout
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextBlock>

#include "BlockHighlight.h"
#include "CustomTextEdit.h"
#include "TextBlockData.h"

using namespace std;

//_______________________________________________________________________
BlockHighlight::BlockHighlight( CustomTextEdit* parent ):
  QObject( parent ),
  Counter( "BlockHighlight" ),
  parent_( parent ),
  enabled_( false ),
  cleared_( true )
{ Debug::Throw( "BlockHighlight::BlockHighlight.\n" ); }

//______________________________________________________________________
void BlockHighlight::clear( void )
{

  if( cleared_ ) return;
  
  // loop over all blocks
  for( QTextBlock block = parent_->document()->begin(); block.isValid(); block = block.next() )
  {
    
    if( parent_->textCursor().block() == block && isEnabled() ) continue;
    
    TextBlockData* data( dynamic_cast<TextBlockData*>( block.userData() ) );
    if( data && data->hasFlag( TextBlock::CURRENT_BLOCK ) ) 
    {
      data->setFlag( TextBlock::CURRENT_BLOCK, false );
      
      // retrieve paragraph rect
      QRectF block_rect( parent_->document()->documentLayout()->blockBoundingRect( block ) );
      block_rect.setWidth( parent_->viewport()->width() );
      parent_->viewport()->update( parent_->toViewport( block_rect.toRect() ) );    
      
    }
    
  }
  
  cleared_ = true;

}

//______________________________________________________________________
void BlockHighlight::highlight( void )
{
  if( !isEnabled() ) return;
  clear();
  timer_.start(50, this );
}

//______________________________________________________________________
void BlockHighlight::timerEvent( QTimerEvent* event )
{
  
  if( event->timerId() != timer_.timerId() ) return QObject::timerEvent( event ); 
  
  // retrieve current block
  QTextBlock block( parent_->textCursor().block() );  
  
  TextBlockData* data = dynamic_cast<TextBlockData*>( block.userData() );
  if( !data )
  {
    data = new TextBlockData();
    block.setUserData( data );
  } else if( data->hasFlag( TextBlock::CURRENT_BLOCK ) ) return;
  
  // need to redo the clear a second time, forced,
  // in case the previous draw action occured after the previous clear.
  cleared_ = false;
  clear();

  // mark block as current
  data->setFlag( TextBlock::CURRENT_BLOCK, true );

  // retrieve block rect, translate to viewport and ask for repaint
  QRectF block_rect( parent_->document()->documentLayout()->blockBoundingRect( block ) );
  block_rect.setWidth( parent_->viewport()->width() );
  parent_->viewport()->update( parent_->toViewport( block_rect.toRect() ) );    

  cleared_ = false;
  
}

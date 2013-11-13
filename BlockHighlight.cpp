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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

/*!
  \file BlockHighlight.cpp
  \brief handles block highlighting.
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextBlock>

#include "BlockHighlight.h"
#include "BlockHighlight.moc"
#include "TextEditor.h"
#include "TextBlockData.h"



//_______________________________________________________________________
BlockHighlight::BlockHighlight( TextEditor* parent ):
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

    TextBlockData* data( static_cast<TextBlockData*>( block.userData() ) );
    if( data && data->hasFlag( TextBlock::CURRENT_BLOCK ) )
    {

      // reset flag
      data->setFlag( TextBlock::CURRENT_BLOCK, false );

      // mark contents dirty to trigger document update
      _updateEditors();

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
  timer_.stop();
  _highlight();

}

//______________________________________________________________________
void BlockHighlight::_highlight( void )
{

  if( !isEnabled() ) return;

  // retrieve current block
  QTextBlock block( parent_->textCursor().block() );

  TextBlockData* data = static_cast<TextBlockData*>( block.userData() );
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

  // mark contents dirty to trigger document update
  // parent_->document()->markContentsDirty(block.position(), block.length()-1);
  _updateEditors();

  cleared_ = false;

}

//______________________________________________________________________
void BlockHighlight::_updateEditors( void )
{

  BASE::KeySet<TextEditor> editors( parent_ );
  editors.insert( parent_ );
  for( BASE::KeySet<TextEditor>::iterator iter = editors.begin(); iter != editors.end(); ++iter )
  { (*iter)->viewport()->update(); }

}

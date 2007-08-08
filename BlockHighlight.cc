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
  \file BlockHighlight.cc
  \brief handles multiple clicks and timeout
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QTextDocument>

#include "BlockHighlight.h"
#include "CustomTextEdit.h"
#include "TextBlockData.h"

using namespace std;

//_______________________________________________________________________
BlockHighlight::BlockHighlight( CustomTextEdit* parent ):
  QObject( parent ),
  Counter( "BlockHighlight" ),
  parent_( parent ),
  timer_( parent ),
  enabled_( false )
{ 
  Debug::Throw( "BlockHighlight::BlockHighlight.\n" );
  timer_.setSingleShot( true );
  timer_.setInterval( 50 );
  connect( &timer_, SIGNAL( timeout() ), SLOT( _highlight() ) );
}

//______________________________________________________________________
void BlockHighlight::reset( void )
{
  Debug::Throw( "BlockHighlight::reset.\n" );
  timer_.stop();
}

//______________________________________________________________________
void BlockHighlight::clear( void )
{
  
  // loop over all blocks
  for( QTextBlock block = parent_->document()->begin(); block.isValid(); block = block.next() )
  {
    
    if( parent_->textCursor().block() == block && isEnabled() ) continue;
    
    TextBlockData* data( dynamic_cast<TextBlockData*>( block.userData() ) );
    if( data && data->isCurrentBlock() ) 
    {
      data->setIsCurrentBlock( false );
      parent_->document()->markContentsDirty(block.position(), block.length()-1);
    }
    
  } 

}

//______________________________________________________________________
void BlockHighlight::highlight( void )
{
  if( !isEnabled() ) return;
  clear();
  timer_.start();
}

//______________________________________________________________________
void BlockHighlight::_highlight( void )
{
  
  // retrieve current block
  QTextBlock block( parent_->textCursor().block() );  
  
  TextBlockData* data = dynamic_cast<TextBlockData*>( block.userData() );
  if( !data )
  {
    data = new TextBlockData();
    block.setUserData( data );
  } else if( data->isCurrentBlock() ) return;
  
  data->setIsCurrentBlock( true );
 
  parent_->document()->markContentsDirty(block.position(), block.length()-1);
  
}

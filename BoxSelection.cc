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
  \file BoxSelection.cc
  \brief Box selection
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QScrollBar>

#include "BoxSelection.h"
#include "CustomTextEdit.h"
#include "XmlOptions.h"

using namespace std;

//________________________________________________________________________
const QString BoxSelection::mimeType( "text/boxSelection" );

//________________________________________________________________________
BoxSelection::BoxSelection( CustomTextEdit* parent ):
  Counter( "BoxSelection" ),
  parent_( parent ),
  enabled_( false ),
  font_width_( 0 ),
  font_height_( 0 ),
  left_margin_( 0 ),
  top_margin_( 0 ),
  state_( EMPTY )
{ Debug::Throw( "BoxSelection::BoxSelection.\n" ); }

//________________________________________________________________________
void BoxSelection::synchronize( const BoxSelection& box )
{
  
  // Debug::Throw( "BoxSelection::synchronize.\n" );
  begin_ = box.begin();
  end_ = box.end();
  state_ = box.state();
  
  QRect old( rect() );
  _updateRect();
  if( state() == STARTED || state() == FINISHED ) { parent_->viewport()->repaint( old.unite( rect() ) ); } 
  else if( state() == EMPTY ) clear();
  
  return; 
  
} 
  
//________________________________________________________________________
void BoxSelection::updateConfiguration( void )
{

  Debug::Throw( "BoxSelection::updateConfiguration.\n" );
  color_ = parent_->palette().color( QPalette::Highlight );
  if( color_.isValid() ) color_.setAlpha( 50 );

  // check if color is valid and font is fixed pitched
  // to not modify the previous attributes if disabled
  // because they are needed to clear any existing selection
  bool fixed( QFontInfo( parent_->font() ).fixedPitch() );
  enabled_ = fixed && color_.isValid();
  if( !isEnabled() ) return;

  // read font attributes
  font_width_ = QFontMetrics( parent_->font() ).width( " " );
  font_height_ = QFontMetrics( parent_->font() ).height();
    
  // retrieve margins
  int right, bottom;
  parent_->getContentsMargins( &left_margin_, &top_margin_, &right, &bottom );

}

//________________________________________________________________________
bool BoxSelection::start( QPoint point )
{
  Debug::Throw( "BoxSelection::start.\n" );
  if( state_ != EMPTY ) return false;

  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );

  begin_ = point;
  end_ = point;
  _updateRect();
  parent_->setTextCursor( parent_->cursorForPosition( cursor_ ) );

  state_ = STARTED;
  return true;
}

//________________________________________________________________________
bool BoxSelection::update( QPoint point )
{
  Debug::Throw( "BoxSelection::update.\n" );
  if( state_ != STARTED ) return false;

  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );

  end_ = point;

  QRect old( rect() );
  _updateRect();
  parent_->viewport()->repaint( old.unite( rect() ) );
  parent_->setTextCursor( parent_->cursorForPosition( cursor_ ) );

  return true;
}

//________________________________________________________________________
bool BoxSelection::finish( QPoint point )
{
  Debug::Throw( "BoxSelection::finish.\n" );
  if( state_ != STARTED ) return false;

  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );

  end_ = point;

  QRect old( rect() );
  _updateRect();
  parent_->viewport()->repaint( old.unite( rect() ) );
  parent_->setTextCursor( parent_->cursorForPosition( cursor_ ) );

  state_ = FINISHED;

  // store and copy to clipboard
  _store();
  toClipboard( QClipboard::Selection );
    
  return true;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{
  Debug::Throw( "BoxSelection::clear.\n" );
  if( state_ != FINISHED ) return false;
  state_ = EMPTY;
  parent_->viewport()->repaint();
  return true;
}

//________________________________________________________________________
bool BoxSelection::toClipboard( const QClipboard::Mode& mode ) const
{
  Debug::Throw(0) << "BoxSelection::toClipboard - mode: " << ( mode == QClipboard::Selection ? "Selection":"Clipboard" ) << endl;
  
  // check if selection mode is available
  if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;

  // check if state is ok
  if( state() != FINISHED ) return false;
  
  // check if stored text makes sense
  if( stored_.isEmpty() ) return false;
  
  // store text into MimeData
  QMimeData *data( new QMimeData() );
  data->setText( stored_ );
  data->setData( mimeType, stored_.toAscii() );
  
  // copy selected text to clipboard
  qApp->clipboard()->setMimeData( data, mode ); 
  
  return true;
  
}

//________________________________________________________________________
bool BoxSelection::fromClipboard( const QClipboard::Mode& mode )
{
  
  Debug::Throw( "BoxSelection::fromClipboard.\n" );
 
  // check mode
  if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;  
  
  // check mime data availability
  const QMimeData* data( qApp->clipboard()->mimeData() );
  if( !data ) 
  {
    Debug::Throw() << "BoxSelection::fromClipboard - no mimeData" << endl;
    return false;
  }
  
  // try retrieve directly from formated input
  if( data->hasFormat( mimeType ) )
  { 
    Debug::Throw() << "BoxSelection::fromClipboard - got formated string" << endl;
    stored_ = QString( data->data( mimeType ) );
    return true;
  } else if( data->hasText() ) {
    
    // retrieve from text and format
    return fromString( data->text() );
  
  }
  
  Debug::Throw() << "BoxSelection::fromClipboard - unable to decode clipboard.\n" << endl;
  return true; 
  
}
  
//________________________________________________________________________
bool BoxSelection::fromString( const QString& input )
{
  
  Debug::Throw( "BoxSelection::fromString.\n" );
  
  // try split
  QStringList input_list( input.split( "\n" ) );

  // retrieve maximum length
  int length(0);
  for( QStringList::const_iterator iter( input_list.begin() ); iter != input_list.end(); iter++ )
  { length = max( length, iter->length() ); }
  
  // fill strings and store
  stored_ = "";
  for( QStringList::const_iterator iter( input_list.begin() ); iter != input_list.end(); iter++ )
  { stored_ += iter->leftJustified( length ) + "\n"; }
  
  return !stored_.isEmpty();
  
}

//________________________________________________________________________
void BoxSelection::_updateRect( void )
{
  int x_min( min( begin_.x(), end_.x() ) );
  int x_max( max( begin_.x(), end_.x() ) );

  int y_min( min( begin_.y(), end_.y() ) );
  int y_max( max( begin_.y(), end_.y() ) );

  QPoint begin( x_min - (x_min%font_width_) + left_margin_ + 2, y_min - (y_min%font_height_) + top_margin_ );
  QPoint end( x_max + font_width_ - (x_max%font_width_) + left_margin_, y_max + font_height_ - (y_max%font_height_) + top_margin_ );

  // decide location of cursor point
  QPoint translation( parent_->horizontalScrollBar()->value(), parent_->verticalScrollBar()->value() );
  cursor_.setX( begin_.x() < end_.x() ? end.x():begin.x());
  cursor_.setY( begin_.y() < end_.y() ? end.y():begin.y()+font_height_);
  cursor_ -= translation;

  // compute rect
  rect_ = QRect( begin, end );

  return;

}

//________________________________________________________________________
void BoxSelection::_store( void )
{
  Debug::Throw( "BoxSelection::_store.\n" );

  // retrieve box selection size
  // the +1 are added because the box is made too small, by design.
  int columns = rect().width() / font_width_;
  int rows = rect().height() / font_height_;
  
  // translate rect
  QRect local( rect() );
  local.translate( -parent_->horizontalScrollBar()->value(), -parent_->verticalScrollBar()->value() );
  
  stored_ = QString();

  // loop over rows
  for( int row = 0; row < rows; row ++ )
  {
    QPoint voffset( 0, font_height_*( row + 1 ) );
    QPoint begin( local.topLeft() + voffset );
    QPoint end( local.topRight() + voffset );
    
    // retrieve cursor at begin popsition
    QTextCursor cursor_begin( parent_->cursorForPosition( begin ) );

    // check if cursor is at end of block, in which case the line is empty
    if( cursor_begin.atBlockEnd() )
    {
      stored_ += QString( columns, ' ') + "\n";
      continue;
    }

    // retrieve cursor at end
    QTextCursor cursor_end( parent_->cursorForPosition( end ) );

    // mover begin cursor to the end
    cursor_begin.setPosition( cursor_end.position(), QTextCursor::KeepAnchor );

    // retrieve selection and complete so that it has the correct width
    stored_ += ( cursor_begin.selectedText() ).leftJustified( columns ) + "\n";

  }
    
  return;

}

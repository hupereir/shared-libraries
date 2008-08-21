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
  \file BoxSelection.cpp
  \brief Box selection
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>

#include "BoxSelection.h"
#include "TextEditor.h"
#include "XmlOptions.h"

using namespace std;

static const unsigned int debug_level = 1;

//________________________________________________________________________
const QString BoxSelection::mimeType = "text/boxSelection";

//________________________________________________________________________
BoxSelection::BoxSelection( TextEditor* parent ):
  Counter( "BoxSelection" ),
  parent_( parent ),
  enabled_( false ),
  font_width_( 0 ),
  font_height_( 0 ),
  left_margin_( 0 ),
  top_margin_( 0 ),
  state_( EMPTY )
{ Debug::Throw( debug_level, "BoxSelection::BoxSelection.\n" ); }

//________________________________________________________________________
void BoxSelection::synchronize( const BoxSelection& box )
{
  
  Debug::Throw( debug_level, "BoxSelection::synchronize.\n" ); 
  
  // check enable state.
  if( !checkEnabled() ) return;

  begin_ = box.begin();
  end_ = box.end();
  state_ = box.state();
    
  QRect old( rect() );
  _updateRect();
  
  parent_->viewport()->update( parent_->toViewport( old.unite( rect() ) ) );
  
  return; 
  
} 
  
//________________________________________________________________________
void BoxSelection::updateConfiguration( void )
{

  Debug::Throw( debug_level, "BoxSelection::updateConfiguration.\n" );
  
  // retrieve box selection color from options
  QColor color;
  if( XmlOptions::get().find( "BOX_SELECTION_COLOR" ) )
  { color = QColor( XmlOptions::get().raw( "BOX_SELECTION_COLOR" ).c_str() ); }
  
  // if invalid, use the normal selection color
  if( !color.isValid() ) color = parent_->palette().color( QPalette::Highlight );
  color_ = color;
    
  // retrieve shading from options
  color.setAlpha( XmlOptions::get().get<double>("BOX_SELECTION_ALPHA")*255/100 );
  brush_ = QBrush( color );

  // additional initialization dependening on whether box selection is enabled or not
  enabled_ = false;
  checkEnabled();
}

//________________________________________________________________________
bool BoxSelection::checkEnabled( void )
{

  Debug::Throw( debug_level, "BoxSelection::checkEnabled.\n" );
  if( isEnabled() ) return true;
  
  /*
  check if color is valid and font is fixed pitched
  to not modify the previous attributes if disabled
  because they are needed to clear any existing selection
  */
  bool fixed( QFontInfo( parent_->font() ).fixedPitch() );
  enabled_ = fixed && color_.isValid();
  Debug::Throw( debug_level ) << "BoxSelection::updateConfiguration - isEnabled: " << isEnabled() << endl;
  if( !isEnabled() ) return false;

  // read font attributes
  font_width_ = QFontMetrics( parent_->font() ).width( " " );
  font_height_ = QFontMetrics( parent_->font() ).height();
    
  // retrieve margins
  int right, bottom;
  parent_->getContentsMargins( &left_margin_, &top_margin_, &right, &bottom );
  return true;
}
  
//________________________________________________________________________
bool BoxSelection::start( QPoint point )
{
  Debug::Throw( debug_level, "BoxSelection::start.\n" );
  if( state_ != EMPTY ) return false;

  // store point
  begin_ = end_ = parent_->fromViewport( point );
  
  // adjust rectangle size
  _updateRect();
  
  // set parent cursor
  parent_->setTextCursor( parent_->cursorForPosition( cursor_ + QPoint(0,-1) ) );

  state_ = STARTED;
  return true;
}

//________________________________________________________________________
bool BoxSelection::update( QPoint point )
{
  Debug::Throw( debug_level, "BoxSelection::update.\n" );
  if( state_ != STARTED ) return false;

  // store end point
  end_ = parent_->fromViewport( point );

  // retrieve old rect
  QRect old( rect() );
  
  // update with new value
  _updateRect();
  
  // update parent 
  parent_->viewport()->update( parent_->toViewport( old.unite( rect() ) ) );
  parent_->setTextCursor( parent_->cursorForPosition( cursor_ + QPoint(0,-1) ) );

  return true;
}

//________________________________________________________________________
bool BoxSelection::finish( QPoint point )
{
  Debug::Throw( debug_level, "BoxSelection::finish.\n" );
  if( state_ != STARTED ) return false;

  update( point );
  state_ = FINISHED;

  // store and copy to clipboard
  _store();
  toClipboard( QClipboard::Selection );
    
  return true;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{
  Debug::Throw( debug_level, "BoxSelection::clear.\n" );
  if( state_ != FINISHED ) return false;

  // change state and redraw
  state_ = EMPTY;
  
  // update parent
  parent_->viewport()->update( parent_->toViewport( rect() ) );
  
  // clear cursors points and rect
  cursors_.clear();
  cursor_ = begin_ = end_ = QPoint();
  rect_ = QRect();
  
  return true;
}


//________________________________________________________________________
QString BoxSelection::toString( void ) const
{
  Debug::Throw( debug_level, "BoxSelection::toString.\n" );
  
  QString out;
  
  // retrieve cursor, copy selected text in output string
  CursorList cursors( cursorList() );
  if( cursorList().empty() ) return out;
  
  // append a new line for all items but the last
  for( int i=0;  i < cursorList().size()-1; i++ )
  { out += cursorList()[i].selectedText().leftJustified( cursors.columnCount() ) + "\n"; }
  
  // append last item
  out += cursorList().back().selectedText().leftJustified( cursors.columnCount() );
    
  return out;
  
}

//________________________________________________________________________
bool BoxSelection::fromString( QString input )
{
  
  Debug::Throw( debug_level, "BoxSelection::fromString.\n" );
  
  // check state
  if( state() != FINISHED ) return false;

  // replace all tab characters by emulated tabs
  input.replace( "\t", parent_->emulatedTabCharacter() );
  
  // try split
  QStringList input_list( input.split( "\n" ) );
  
  // retrieve maximum length
  int columns(0);
  for( int i=0; i < input_list.size(); i++ )
  { columns = max( columns, input_list[i].length() ); }

  // if there are more lines in current box than in the selection, fill with blank lines
  for( int i = input_list.size(); i < cursors_.size(); i++ )
  { input_list.push_back( QString( columns, ' ' ) ); }
  
  // loop over items and cursors, replace strings
  QTextCursor cursor( parent_->textCursor() );
  cursor.beginEditBlock();
  for( int i=0; i < min( input_list.size(), cursors_.size() ); i++ )
  {
    
    cursor.setPosition( cursors_[i].anchor() );

    // if cursor has no selection, one need to make sur that there is enough white spaces in the line before copying the new string
    // this hack is necessary because for some reason cursor.movePosition( QTextCursor::StartOfLine ) fails.
    int extra_length = cursors_.firstColumn() ;
    int old_bottom( parent_->cursorRect( cursor ).bottom() );
    int new_bottom( 0 );
    while( cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor ) && (new_bottom =  parent_->cursorRect( cursor ).bottom() ) == old_bottom ) 
    {
      extra_length--;
      old_bottom = new_bottom;
    }
    
    // if( !cursor.movePosition( QTextCursor::StartOfLine, QTextCursor::KeepAnchor ) )
    // { Debug::Throw() << "BoxSelection::fromString - unable to move cursor" << endl; }    
    // int extra_length = cursors_.firstColumn() - cursor.anchor() + cursor.position();
        
    // move to current cursor end of the selection
    cursor.setPosition( cursors_[i].position(), QTextCursor::KeepAnchor );
    
    // insert new text
    cursor.insertText( input_list[i].rightJustified( extra_length + input_list[i].size() ).leftJustified( extra_length + columns ) );

  }
  
  // if there are more lines in input_list than in current selection, insert new lines
  for( int i = cursors_.size(); i < input_list.size(); i++ )
  {
    cursor.insertText( QString( '\n' ) + 
      input_list[i]
      .rightJustified( cursors_.firstColumn() + input_list[i].size() )
      .leftJustified( cursors_.firstColumn() + columns ) );
  }
  
  cursor.endEditBlock();
  parent_->setTextCursor( cursor );
  
  return true;
  
}

//________________________________________________________________________
bool BoxSelection::toClipboard( const QClipboard::Mode& mode ) const
{
  Debug::Throw(debug_level) << "BoxSelection::toClipboard - mode: " << ( mode == QClipboard::Selection ? "Selection":"Clipboard" ) << endl;
  
  // check if selection mode is available
  if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;

  // check if state is ok
  if( state() != FINISHED ) return false;
    
  // check if stored text makes sense
  QString selection( toString() );
  if( selection.isEmpty() ) return false;
  
  // store text into MimeData
  QMimeData *data( new QMimeData() );
  data->setText( selection );
  data->setData( mimeType, selection.toAscii() );
  
  // copy selected text to clipboard
  qApp->clipboard()->setMimeData( data, mode ); 
  
  return true;
  
}

//________________________________________________________________________
bool BoxSelection::fromClipboard( const QClipboard::Mode& mode )
{
  
  Debug::Throw( debug_level, "BoxSelection::fromClipboard.\n" );
 
  // check mode
  if( mode == QClipboard::Selection && !qApp->clipboard()->supportsSelection() ) return false;  
  
  // check mime data availability
  const QMimeData* data( qApp->clipboard()->mimeData() );
  if( !data ) 
  {
    Debug::Throw(debug_level) << "BoxSelection::fromClipboard - no mimeData" << endl;
    return false;
  }
  
  // try retrieve directly from formated input
  return data->hasText() ? fromString( data->text() ) : false;
  
}

//________________________________________________________________________
bool BoxSelection::removeSelectedText( void ) const
{
  Debug::Throw( debug_level, "BoxSelection::removeSelectedText.\n" );
  
  // check if state is ok
  if( state() != FINISHED || cursorList().empty() ) return false;
  
  QTextCursor stored( parent_->textCursor() );
  QTextCursor cursor( parent_->textCursor() );
  cursor.beginEditBlock();
  for( CursorList::const_iterator iter = cursorList().begin(); iter != cursorList().end(); iter++ )
  {
    
    // select line and remove
    cursor.setPosition( iter->anchor() );
    cursor.setPosition( iter->position(), QTextCursor::KeepAnchor );
    cursor.removeSelectedText();
  
  }
  
  cursor.endEditBlock();
  
  // restore cursor
  if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
  { 
    stored.setPosition( cursorList().front().anchor() ); 
  } else stored.setPosition( cursor.position() );

  parent_->setTextCursor( stored );
  
  return true;
  
}

//________________________________________________________________________
bool BoxSelection::toUpper( void )
{
  Debug::Throw( debug_level, "BoxSelection::toUpper.\n" );
  
  // check if state is ok
  if( state() != FINISHED || cursorList().empty() ) return false;
  
  QTextCursor stored( parent_->textCursor() );
  QTextCursor cursor( parent_->textCursor() );
  cursor.beginEditBlock();
  for( CursorList::const_iterator iter = cursorList().begin(); iter != cursorList().end(); iter++ )
  {
    
    // select line and remove
    cursor.setPosition( iter->anchor() );
    cursor.setPosition( iter->position(), QTextCursor::KeepAnchor );
    cursor.insertText( cursor.selectedText().toUpper() );
  
  }
  
  cursor.endEditBlock();
  
  // restore cursor
  if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
  { 
    stored.setPosition( cursorList().front().anchor() ); 
  } else stored.setPosition( cursor.position() );

  parent_->setTextCursor( stored );
  
  _store();
  toClipboard( QClipboard::Selection );
  
  return true;
  
}

//________________________________________________________________________
bool BoxSelection::toLower( void )
{
  Debug::Throw( debug_level, "BoxSelection::toLower.\n" );
  
  // check if state is ok
  if( state() != FINISHED || cursorList().empty() ) return false;
  
  QTextCursor stored( parent_->textCursor() );
  QTextCursor cursor( parent_->textCursor() );
  cursor.beginEditBlock();
  for( CursorList::const_iterator iter = cursorList().begin(); iter != cursorList().end(); iter++ )
  {
    
    // select line and remove
    cursor.setPosition( iter->anchor() );
    cursor.setPosition( iter->position(), QTextCursor::KeepAnchor );
    cursor.insertText( cursor.selectedText().toLower() );
  
  }
  
  cursor.endEditBlock();
  
  // restore cursor
  if( stored.position() == cursorList().front().position() || stored.position() == cursorList().front().anchor() )
  { 
    stored.setPosition( cursorList().front().anchor() ); 
  } else stored.setPosition( cursor.position() );

  parent_->setTextCursor( stored );
  
  _store();
  toClipboard( QClipboard::Selection );
  return true;
  
}

//________________________________________________________________________
void BoxSelection::_updateRect( void )
{
  Debug::Throw( debug_level, "BoxSelection::_updateRect.\n" );
  int x_min( min( begin_.x(), end_.x() ) );
  int x_max( max( begin_.x(), end_.x() ) );

  int y_min( min( begin_.y(), end_.y() ) );
  int y_max( max( begin_.y(), end_.y() ) );
  
  QPoint begin( x_min - (x_min%font_width_) + left_margin_, y_min - (y_min%font_height_) + top_margin_ );
  QPoint end( x_max - (x_max%font_width_) + left_margin_, y_max + font_height_ - (y_max%font_height_) + top_margin_ );

  // decide location of cursor point
  cursor_.setX( begin_.x() < end_.x() ? end.x():begin.x());
  cursor_.setY( begin_.y() < end_.y() ? end.y():begin.y()+font_height_);
  cursor_ = parent_->toViewport( cursor_ );

  // compute rect
  rect_ = QRect( begin, end );

  return;

}

//________________________________________________________________________
void BoxSelection::_store( void )
{
  Debug::Throw( debug_level, "BoxSelection::_store.\n" );

  // retrieve box selection size
  int first_column = rect().left() / font_width_;
  int columns = rect().width() / font_width_;
  int rows = rect().height() / font_height_;
  
  Debug::Throw() << "BoxSelection::_store - [" << first_column << "," << columns << "," << rows << "]" << endl;
  
  // translate rect
  QRect local( parent_->toViewport( rect() ) );  
  cursors_ = CursorList( first_column, columns );
  Debug::Throw() << "BoxSelection::_store - rect left: " << rect().left() << " right: " << rect().right() << endl;
  Debug::Throw() << "BoxSelection::_store - local left: " << local.left() << " right: " << local.right() << endl;
  for( int row = 0; row < rows; row ++ )
  {

    // vertical offset for this row
    QPoint voffset( 0, (int)(font_height_*( 0.5+row )) );
    QPoint begin( local.topLeft() + voffset );
    QPoint end( local.topRight() + voffset );
    
    Debug::Throw() << "BoxSelection::_store -"
      << " begin: (" << begin.x() << "," << begin.y() << ")"
      << " end: (" << end.x() << "," << end.y() << ")"
      << endl;
    
    QTextCursor cursor( parent_->cursorForPosition( begin ) );
    Debug::Throw() 
      << "BoxSelection::_store -"
      << " begin: " << parent_->cursorForPosition( begin ).position() 
      << " end: " << parent_->cursorForPosition( end ).position() 
      << endl;
    
    // check if cursor is at end of block, and move at end
    if( !cursor.atBlockEnd() )
    { cursor.setPosition( parent_->cursorForPosition( end ).position(), QTextCursor::KeepAnchor ); }
    
    // store in list
    cursors_.push_back( cursor );
    
  }
    
  return;

}

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
* ANY WARRANTY; witout even the implied warranty of MERCHANTABILITY or
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
  \file TextEditor.cpp
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QClipboard>
#include <QMenu>
#include <QPainter>
#include <QProgressDialog>
#include <QRegExp>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextStream>

#include "BaseIcons.h"
#include "BaseReplaceDialog.h"
#include "InformationDialog.h"
#include "Color.h"
#include "TextEditor.h"
#include "CustomTextDocument.h"
#include "BaseFindDialog.h"
#include "IconEngine.h"
#include "LineNumberDisplay.h"
#include "QtUtil.h"
#include "SelectLineDialog.h"
#include "Singleton.h"
#include "TextBlockData.h"
#include "TextSeparator.h"
#include "XmlOptions.h"

using namespace std;

//______________________________________________
TextSelection& TextEditor::lastSelection( void )
{
  static TextSelection selection;
  return selection;
}

//______________________________________________
TextEditor::TextEditor( QWidget *parent ):
  QTextEdit( parent ),
  Counter( "TextEditor" ),
  find_dialog_( 0 ),
  replace_dialog_( 0 ),
  select_line_dialog_( 0 ),
  line_number_display_( 0 ),
  left_margin_( 0 ),
  draw_vertical_line_( false ),
  active_( false ),
  wrap_from_options_( true ),
  line_number_from_options_( true ),
  has_tab_emulation_( false ),
  show_line_number_action_( 0 ),
  synchronize_( false ),
  box_selection_( this ),
  remove_line_buffer_( this ),
  click_counter_( this ),
  margin_dirty_( true )
{

  Debug::Throw( "TextEditor::TextEditor.\n" );

  // set customized document
  CustomTextDocument* document( new CustomTextDocument(0) );
  BASE::Key::associate( this, document );
  setDocument( document );

  // paragraph highlight
  block_highlight_ = new BlockHighlight( this );

  // actions
  _installActions();

  // line number
  line_number_display_ = new LineNumberDisplay( this );

  // signal to make sure selectionsynchronized is  between clones
  connect( this, SIGNAL( cursorPositionChanged() ), &blockHighlight(), SLOT( highlight() ) );
  connect( this, SIGNAL( copyAvailable( bool ) ), SLOT( _updateSelectionActions( bool ) ) );
  connect( this, SIGNAL( selectionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( selectionChanged() ), SLOT( _updateClipboard() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

  // track changes of block counts
  connect( TextEditor::document(), SIGNAL( blockCountChanged( int ) ), SLOT( _blockCountChanged( int ) ) );
  connect( TextEditor::document(), SIGNAL( contentsChanged() ), SLOT( _setMarginDirty() ) );

  // update configuration
  _updateConfiguration();
  _blockCountChanged(0);
}

//________________________________________________
TextEditor::~TextEditor( void )
{

  Debug::Throw() << "TextEditor::~TextEditor - key: " << key() << endl;

  // cast document
  CustomTextDocument* document( dynamic_cast<CustomTextDocument*>( TextEditor::document() ) );
  if( document && BASE::KeySet<TextEditor>( document ).size() == 1 ) document->deleteLater();

  // update associates synchronization flags
  BASE::KeySet<TextEditor> editors( this );

  // nothing to be done if no associates
  if( editors.empty() ) return;

  // keep position of current cursor
  int position( textCursor().position() );
  int anchor( textCursor().anchor() );

  // need to reset Text document
  // to avoid deletion while deleting this editor
  setDocument( new QTextDocument() );

  // keep reference to first associate
  TextEditor &editor( **editors.begin() );

  // recreate an appropriate cursor
  // this is dangerous
  QTextCursor cursor( editor.document() );
  cursor.setPosition( anchor );
  cursor.setPosition( position, QTextCursor::KeepAnchor );
  editor.setTextCursor( cursor );

  // turn off synchronization
  if( editors.size() == 1 ) editor.setSynchronized( false );
  Debug::Throw() << "TextEditor::~TextEditor - done." << endl;

}

//________________________________________________
int TextEditor::blockCount( void ) const
{
  Debug::Throw( "TextEditor::blockCount.\n" );

  int count = 0;
  for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next() ) 
  { count += blockCount( block ); }
  return count;
}

//________________________________________________
bool TextEditor::isCursorVisible( void ) const
{
  
  QRect cursor_rect( cursorRect() );
  QRect rect( viewport()->rect() );
  return rect.intersects( cursor_rect );
  
}

//________________________________________________
TextPosition TextEditor::textPosition( void )
{

  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );

  // calculate index
  TextPosition out;
  
  #if QT_VERSION >= 0x040200
  
  // direct access as introduced in Qt 4.2
  out.index() = cursor.columnNumber();
  out.paragraph() = cursor.blockNumber();
  
  #else
  
  // slow access for prior versions of Qt
  out.index() = cursor.position() - block.position();
  while( block.isValid() )
  {
    block = block.previous();
    out.paragraph()++;
  }

  // need to decrement once
  out.paragraph()--;
  
  #endif
  
  return out;

}

//________________________________________________
TextPosition TextEditor::positionFromIndex( const int& index ) const
{
  Debug::Throw( "TextEditor::positionFromIndex.\n" );

  TextPosition out;

  // retrieve block matching position
  QTextBlock block( document()->findBlock( index ) );
  if( block.isValid() ) out.index() = index - block.position();
  else {

    // if no valid block is found, return position of last character in text
    block = document()->end();
    out.index() = block.length();

  }

  // rewind to begin of document to get paragraph index
  while( (block = block.previous()).isValid() ) out.paragraph()++;

  return out;

}

//________________________________________________
int TextEditor::indexFromPosition( const TextPosition& position ) const
{

  Debug::Throw( "TextEditor::indexFromPosition.\n" );

  // advance until paragraph is matched
  QTextBlock block( document()->begin() );
  int paragraph(0);
  while( paragraph < position.paragraph() && block.isValid() )
  {
    paragraph++;
    block = block.next();
  }

  // if block is valid advance until index is smaller than current block size
  int index = position.index();
  if( block.isValid() )
  {
    while( index > block.length() && block.isValid() )
    {
      block = block.next();
      index -= block.length();
    }
  }

  // if block is valid return found index
  if( block.isValid() ) return block.position() + index;
  else {
    block = document()->end();
    return block.position() + block.length();
  }

}

//________________________________________________
void TextEditor::setPlainText( const QString& text )
{
  Debug::Throw( "TextEditor::setPlainText.\n" );

  lineNumberDisplay().clear();
  
  bool enabled( blockHighlight().isEnabled() );
  blockHighlight().setEnabled( false );
  QTextEdit::setPlainText( text );
  blockHighlight().setEnabled( enabled );

}

//________________________________________________
void TextEditor::setHtml( const QString& text )
{
  Debug::Throw( "TextEditor::setHtml.\n" );

  bool enabled( blockHighlight().isEnabled() );
  blockHighlight().setEnabled( false );
  QTextEdit::setHtml( text );
  blockHighlight().setEnabled( enabled );
  
}

//________________________________________________
void TextEditor::selectWord( void )
{  Debug::Throw( "TextEditor::selectWord.\n" );

  // retrieve text cursor, block and text
  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );
  QString text( cursor.block().text() );

  // retrieve local cursor position in block
  int local_position( cursor.position() - block.position() );
  int begin = local_position;
  int end = local_position;

  // parse text
  if( TextSeparator::get().base().find( text[begin] ) != TextSeparator::get().base().end() )
  {

    // see if cursor is in base separator list
    while( begin > 0 &&  TextSeparator::get().base().find( text[begin-1] ) != TextSeparator::get().base().end() ) begin--;
    while( end < text.size() && TextSeparator::get().base().find( text[end] ) != TextSeparator::get().base().end() ) end++;

  } else if( TextSeparator::get().extended().find( text[begin] ) != TextSeparator::get().extended().end() ) {

    // see if cursor is in extended separator list
    while( begin > 0 &&  TextSeparator::get().extended().find( text[begin-1] ) != TextSeparator::get().extended().end() ) begin--;
    while( end < text.size() && TextSeparator::get().extended().find( text[end] ) != TextSeparator::get().extended().end() ) end++;

  } else {

    // cursor is in word
    while( begin > 0 &&  TextSeparator::get().all().find( text[begin-1] ) == TextSeparator::get().all().end() ) begin--;
    while( end < (int)text.size() && TextSeparator::get().all().find( text[end] ) == TextSeparator::get().all().end() ) end++;

  }

  // move cursor to begin of selection
  for( ;begin < local_position; local_position-- ) { cursor.movePosition( QTextCursor::Left, QTextCursor::MoveAnchor ); }
  for( ;local_position < end; local_position++ ) { cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor ); }

  // assign cursor to Text editor
  setTextCursor( cursor );

  return;

}

//________________________________________________
void TextEditor::selectLine( void )
{
  Debug::Throw( "TextEditor::selectLine.\n" );

  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );
  int begin( block.position() );
  int end( block.position() + block.length() );
  if( !block.next().isValid() ) end--;

  cursor.setPosition( begin );
  cursor.setPosition( end, QTextCursor::KeepAnchor );

  // assign cursor to text editor and make sure it is visible
  setTextCursor( cursor );
  ensureCursorVisible();
  
  // updateClipboard need to be called manually
  // because somehow the selectionChanged signal is not caught.
  _updateClipboard();
  
  return;

}


//________________________________________________
void TextEditor::mergeCurrentCharFormat( const QTextCharFormat& format )
{
  
  static QRegExp regexp( "\\s+$" ); 
  
  QTextCursor cursor( textCursor() );
  if( cursor.hasSelection() )
  {

    // get selection, look for trailing spaces
    QString text( cursor.selectedText() );
    if( regexp.indexIn( text ) >= 0 )
    { 
      
      // create local cursor, copy current, in proper order
      QTextCursor local( document() );
      local.beginEditBlock();
      local.setPosition( min( cursor.position(), cursor.anchor() ), QTextCursor::MoveAnchor );
      local.setPosition( max( cursor.position(), cursor.anchor() ), QTextCursor::KeepAnchor );
      local.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, regexp.matchedLength() );
      
      local.mergeCharFormat( format );
      local.endEditBlock();
      
      return;
      
    }

  } else if( _boxSelection().state() == BoxSelection::FINISHED ) { 
    
    // process box selection
    // _boxSelection().setCharFormat( format ); 
    _boxSelection().mergeCharFormat( format ); 
    return;
    
  }  
  
  QTextEdit::mergeCurrentCharFormat( format );
  return;

}

//________________________________________________
void TextEditor::synchronize( TextEditor* editor )
{
  Debug::Throw( "TextEditor::synchronize.\n" );

  // retrieve and cast old document
  CustomTextDocument* document( dynamic_cast<CustomTextDocument*>( QTextEdit::document() ) );

  // assign new document and associate
  setDocument( editor->document() );
  BASE::Key::associate( this, dynamic_cast<CustomTextDocument*>( editor->document() ) );

  // delete old document, if needed
  if( document && BASE::KeySet<TextEditor>( document ).size() == 1 ) delete document;

  // set synchronization flag
  editor->setSynchronized( true );
  setSynchronized( true );

  // synchronize cursor position
  setTextCursor( editor->textCursor() );

  // restore scrollbar positions
  horizontalScrollBar()->setValue( editor->horizontalScrollBar()->value() );
  verticalScrollBar()->setValue( editor->verticalScrollBar()->value() );

  // synchronize tab emulation
  _setTabSize( editor->emulatedTabCharacter().size() );
  tabEmulationAction().setChecked( editor->tabEmulationAction().isChecked() );

  // synchronize wrap mode
  wrapModeAction().setChecked( editor->wrapModeAction().isChecked() );
  
  // track changes of block counts
  lineNumberDisplay().synchronize( &editor->lineNumberDisplay() );
  connect( TextEditor::document(), SIGNAL( blockCountChanged( int ) ), SLOT( _blockCountChanged( int ) ) );
  connect( TextEditor::document(), SIGNAL( contentsChanged() ), SLOT( _setMarginDirty() ) );

  // margin
  _setLeftMargin( editor->_leftMargin() );
  
  Debug::Throw( "TextEditor::synchronize - done.\n" );

  return;

}

//_____________________________________________________________________
bool TextEditor::setActive( const bool& active )
{
  
  Debug::Throw( "TextEditor::setActive.\n" );
  
  // check if value is changed
  if( isActive() == active ) return false;
  active_ = active;
  return true;
  
}

//__________________________________________________________________
void TextEditor::showReplacements( const unsigned int& counts )
{

  Debug::Throw( "TextEditor::showReplacements.\n" );

  QString buffer;
  QTextStream stream( &buffer );
  if( !counts ) stream << "String not found.";
  else if( counts == 1 ) stream << "1 replacement performed";
  else stream << counts << " replacements performed";
  InformationDialog( this, buffer ).centerOnWidget( qApp->activeWindow() ).exec();
  
  return;

}


//__________________________________________________________________
void TextEditor::setReadOnly( bool readonly )
{
  Debug::Throw( "TextEditor::setReadOnly.\n" );
  QTextEdit::setReadOnly( readonly );
  _updateReadOnlyActions( readonly );
  if( readonly ) document()->setModified( false );
}

//____________________________________________________________________
void TextEditor::resetUndoRedoStack( void )
{
  if( isReadOnly() || !document()->isUndoRedoEnabled() ) return;
  
  Debug::Throw(" TextEditor::resetUndoRedoStack.\n");
  document()->setUndoRedoEnabled( false );
  document()->setUndoRedoEnabled( true );

}

//______________________________________________________________________________
void TextEditor::installContextMenuActions( QMenu& menu, const bool& all_actions )
{

  Debug::Throw( "TextEditor::installContextMenuActions.\n" );

  // wrapping
  menu.addAction( &showLineNumberAction() );
  menu.addAction( &wrapModeAction() );
  menu.addSeparator();

  if( all_actions )
  {
    menu.addAction( undo_action_ );
    menu.addAction( redo_action_ );
    menu.addSeparator();
  }
  
  menu.addAction( cut_action_ );
  menu.addAction( copy_action_ );
  menu.addAction( paste_action_ );
  menu.addAction( clear_action_ );
  menu.addSeparator();

  menu.addAction( select_all_action_ );
  menu.addAction( upper_case_action_ );
  menu.addAction( lower_case_action_ );
  menu.addSeparator();

  menu.addAction( find_action_ );
  if( all_actions )
  {
    menu.addAction( find_again_action_ );
    menu.addAction( find_selection_action_);
    menu.addSeparator();
  }
  
  menu.addAction( replace_action_ );
  
  if( all_actions )
  {
    menu.addAction( replace_again_action_ );
    menu.addAction( goto_line_action_);
  }
  
  return;
  
}

//___________________________________________________________________________
void TextEditor::setBackground( QTextBlock block, const QColor& color )
{

  Debug::Throw( "TextEditor::setBackground.\n" );

  // try retrieve data or create
  TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
  if( !data ) block.setUserData( data = new TextBlockData() );

  // try assign color
  if( data->setBackground( color ) && updatesEnabled() )
  { document()->markContentsDirty(block.position(), block.length()-1); }
  return;

}

//___________________________________________________________________________
void TextEditor::clearBackground( QTextBlock block )
{

  Debug::Throw( "TextEditor::clearBackground.\n" );
  TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
  if( data && data->hasFlag( TextBlock::HAS_BACKGROUND ) && data->setBackground( QColor() ) && updatesEnabled()  )
  { document()->markContentsDirty(block.position(), block.length()-1); }

  return;
}

//___________________________________________________________________________
void TextEditor::clearAllBackgrounds( void )
{
  Debug::Throw( "TextEditor::clearAllBackgrounds.\n" );
  for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next() )
  { clearBackground( block ); }
}

//________________________________________________
void TextEditor::cut( void )
{

  Debug::Throw( "TextEditor::cut.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  if( _boxSelection().state() == BoxSelection::FINISHED )
  {
    _boxSelection().toClipboard( QClipboard::Clipboard );
    _boxSelection().removeSelectedText();
    _boxSelection().clear();
    emit copyAvailable( false );
  } else QTextEdit::cut();

  return;
}

//________________________________________________
void TextEditor::copy( void )
{
  Debug::Throw( "TextEditor::copy.\n" );
  if( _boxSelection().state() == BoxSelection::FINISHED ) _boxSelection().toClipboard( QClipboard::Clipboard );
  else QTextEdit::copy();
}

//________________________________________________
void TextEditor::paste( void )
{

  Debug::Throw( "TextEditor::paste.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  if( _boxSelection().state() == BoxSelection::FINISHED )
  {
    _boxSelection().fromClipboard( QClipboard::Clipboard );
    _boxSelection().clear();
  } else QTextEdit::paste();
  
}

//________________________________________________
void TextEditor::upperCase( void )
{
  Debug::Throw( "TextEditor::upperCase.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  QTextCursor cursor( textCursor() );
  if( cursor.hasSelection() ) 
  { 
    
    // process standard selection    
    cursor.insertText( cursor.selectedText().toUpper() ); 
    
  } else if( _boxSelection().state() == BoxSelection::FINISHED ) { 
    
    // process box selection
    _boxSelection().toUpper(); 
  
  }  
    
  return;

}

//________________________________________________
void TextEditor::lowerCase( void )
{

  Debug::Throw( "TextEditor::lowerCase.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  QTextCursor cursor( textCursor() );

  if( cursor.hasSelection() ) 
  { 
    
    // process standard selection    
    cursor.insertText( cursor.selectedText().toLower() ); 
    
  } else if( _boxSelection().state() == BoxSelection::FINISHED ) { 
    
    // process box selection
    _boxSelection().toLower(); 
    
  }  
  
  return;
  
}

//______________________________________________________________________
void TextEditor::find( TextSelection selection )
{
  Debug::Throw( "TextEditor::find.\n" );
  bool found( selection.flag( TextSelection::BACKWARD ) ? _findBackward( selection, true ):_findForward( selection, true ) ); 
  if( found ) emit matchFound();
  else emit noMatchFound();
}

//______________________________________________________________________
void TextEditor::findSelectionForward( void )
{ 
  Debug::Throw( "TextEditor::findSelectionForward.\n" );
  _findForward( selection(), true ); 
}

//______________________________________________________________________
void TextEditor::findSelectionBackward( void )
{ 
  Debug::Throw( "TextEditor::findSelectionBackward.\n" );
  _findBackward( selection(), true ); 
}

//______________________________________________________________________
void TextEditor::findAgainForward( void )
{ 
  Debug::Throw( "TextEditor::findAgainForward.\n" );
  _findForward( lastSelection(), true ); 
}

//______________________________________________________________________
void TextEditor::findAgainBackward( void )
{ 
  Debug::Throw( "TextEditor::findAgainBackward.\n" );
  _findBackward( lastSelection(), true ); 
}

//______________________________________________________________________
void TextEditor::replace( TextSelection selection )
{

  Debug::Throw( "TextEditor::replace.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  // see if current selection match
  // perform replacement if yes
  QTextCursor cursor( textCursor() );
  bool accepted( true );
  accepted &= cursor.hasSelection();
  if( selection.flag( TextSelection::REGEXP ) )
  { 
    accepted &= QRegExp( selection.text() ).exactMatch( cursor.selectedText() );
  } else {
    
    accepted &= ( !cursor.selectedText().compare(
      selection.text(),
      selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) );
    
  }

  if( accepted ) 
  {
    cursor.insertText( selection.replaceText() );
    setTextCursor( cursor );
  }

  // try find next occurence
  find( selection );
  return;

}

//______________________________________________________________________
void TextEditor::replaceAgainForward( void )
{
  Debug::Throw( "TextEditor::replaceAgainForward.\n" );
  TextSelection selection( lastSelection() );
  selection.setFlag( TextSelection::BACKWARD, false );
  replace( selection );
}

//______________________________________________________________________
void TextEditor::replaceAgainBackward( void )
{
  Debug::Throw( "TextEditor::replaceAgainBackward.\n" );
  TextSelection selection( lastSelection() );
  selection.setFlag( TextSelection::BACKWARD, true );
  replace( selection );
}

//______________________________________________________________________
unsigned int TextEditor::replaceInSelection( TextSelection selection, const bool& show_dialog )
{
  
  Debug::Throw( "TextEditor::replaceInSelection.\n" );
  
  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return 0;
  
  // progress dialog
  if( show_dialog ) _createProgressDialog();
  
  unsigned int counts(0);
  
  if( _boxSelection().state() == BoxSelection::FINISHED )
  {
    
    Debug::Throw( "TextEditor::replaceInSelection - box selection.\n" );
    BoxSelection::CursorList cursors( _boxSelection().cursorList() );
    for( BoxSelection::CursorList::iterator iter = cursors.begin(); iter != cursors.end(); iter++ )
    { counts += _replaceInRange( selection, *iter, MOVE ); }
    
    _boxSelection().clear();

  } else {
    Debug::Throw( "TextEditor::replaceInSelection - normal selection.\n" );
    QTextCursor cursor( textCursor() );
    counts = _replaceInRange( selection, cursor, EXPAND );
  }

  Debug::Throw( "TextEditor::replaceInSelection - done.\n" );
  if( show_dialog ) showReplacements( counts );
  return counts;

}

//______________________________________________________________________
unsigned int TextEditor::replaceInWindow( TextSelection selection, const bool& show_dialog )
{

  Debug::Throw( "TextEditor::replaceInWindow.\n" );

  // progress dialog
  if( show_dialog ) _createProgressDialog();
  
  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return 0;

  QTextCursor cursor( textCursor() );
  cursor.movePosition( QTextCursor::Start );
  cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
  unsigned int counts( _replaceInRange( selection, cursor, EXPAND ) );

  if( show_dialog ) showReplacements( counts );
  return counts;

}

//________________________________________________
void TextEditor::selectLine( int index )
{

  Debug::Throw() << "TextEditor::selectLine - index: " << index << endl;
  int local_index( 0 );
  QTextBlock block = document()->begin();
  for( ;local_index < index && block.isValid(); block = block.next(), local_index++ )
  {}

  if( block.isValid() )
  {
    // create cursor at begin of block. move to end of block, keeping anchor unchanged
    QTextCursor cursor( block );
    cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );

    // assign to editor
    setTextCursor( cursor );

  } else setTextCursor( QTextCursor( document()->end() ) );

  return;

}

//________________________________________________
void TextEditor::removeLine()
{

  Debug::Throw( "TextEditor::removeLine.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );

  // create cursor selection, depending on whether next block is valid or not
  if( cursor.block().next().isValid() ) 
  {
    cursor.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
    remove_line_buffer_.append( cursor.selectedText() );
  } else {

    // move to previous character
    if( cursor.block().previous().isValid() ) 
    {
      cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor );
      cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
    }
    
    // move to end of current block
    cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
    remove_line_buffer_.append( cursor.selectedText() );
  }
    
  setUpdatesEnabled( false );
  setTextCursor( cursor );
  cut();
  setUpdatesEnabled( true );

}

//________________________________________________
void TextEditor::clear( void )
{
  Debug::Throw( "TextEditor::clear.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;
  
  setUpdatesEnabled( false );
  selectAll();
  cut();  
  setUpdatesEnabled( true );
  
}

//_______________________________________________________
bool TextEditor::event( QEvent* event )
{
  
  switch (event->type()) 
  {
    
    case QEvent::Paint:
    if( _leftMargin() ) 
    {
      
      // case event
      QPaintEvent* paint_event( static_cast<QPaintEvent*>(event) );

      // paint margins
      QPainter painter( this );
      painter.setClipRect( paint_event->rect() );
      _drawMargins( painter );
      painter.end();
      
      // update dirty flag
      // this is done only if the entire margin is redrawn
      if( paint_event->rect().contains( _marginRect() ) )
      { _setMarginDirty( false ); }

    }
    break;
        
    case QEvent::Wheel:
    {
      QWheelEvent *wheel_event( static_cast<QWheelEvent*>(event) );
      if( QRect( frameWidth(),  frameWidth(), _leftMargin(), height() ).contains( wheel_event->pos() ) )
      { qApp->sendEvent( viewport(), event ); }
      return false;
    }

    default: break;
  }
    
  return QTextEdit::event( event );

}

//________________________________________________
void TextEditor::enterEvent( QEvent* event )
{

  #if QT_VERSION < 0x040200
  Debug::Throw( "TextEditor::enterEvent.\n" );
  _updateClipboardActions( QClipboard::Clipboard );
  _updateClipboardActions( QClipboard::Selection );
  #endif
  
  QTextEdit::enterEvent( event );
  
}
  
//________________________________________________
void TextEditor::mousePressEvent( QMouseEvent* event )
{

  Debug::Throw( "TextEditor::mousePressEvent.\n" );

  if( event->button() == Qt::MidButton ) 
  { Debug::Throw( "TextEditor::mousePressEvent - middle mouse button.\n" ); }
  
  // check button
  if( event->button() == Qt::LeftButton )
  {

    // increment multiple clicks
    QTextCursor cursor( textCursor() );
    switch( click_counter_.increment( cursorForPosition( event->pos() ).position() ) )
    {

      case 1:

      // if single click in existing box selection, store drag position
      if(
        event->modifiers() == Qt::NoModifier &&
        _boxSelection().state() == BoxSelection::FINISHED &&
        _boxSelection().rect().contains( fromViewport( event->pos() ) )
        )
      {
        // store position for drag
        drag_start_ = event->pos();
        return QTextEdit::mousePressEvent( event );
      }

      // if single click outside of existing box selection, clear the selection
      if( event->button() == Qt::LeftButton && _boxSelection().state() == BoxSelection::FINISHED )
      {

        _boxSelection().clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );

      }

      // if single click and Control key pressed, start a new box selection
      if( event->modifiers() == Qt::ControlModifier  )
      {

        // try re-enable box selection in case font has changed
        if( _boxSelection().checkEnabled() )
        {

          _boxSelection().start( event->pos() );

          // synchronize with other editors
          _synchronizeBoxSelection();
          return;
        }

      }

      return QTextEdit::mousePressEvent( event );
      break;

      case 2:
      selectWord();
      break;

      case 3:
      selectLine();
      break;

      case 4:
      selectAll();
      _synchronizeSelection();
      _updateClipboard();
      break;

      default:
      event->ignore();
      break;
    }

    return;

  }

  QTextEdit::mousePressEvent( event );

  // for mid button, locate cursor at new position
  if(  event->button() == Qt::MidButton )
  { setTextCursor( cursorForPosition( event->pos() ) ); }

}

//________________________________________________
void TextEditor::mouseDoubleClickEvent( QMouseEvent* event )
{

  Debug::Throw( "TextEditor::mouseDoubleClickEvent.\n" );

  // check button
  if( event->button() == Qt::LeftButton ) mousePressEvent( event );
  else QTextEdit::mouseDoubleClickEvent( event );
  return;

}

//________________________________________________
void TextEditor::mouseMoveEvent( QMouseEvent* event )
{

  Debug::Throw( "TextEditor::mouseMoveEvent.\n" );

  // see if there is a box selection in progress
  if( event->buttons() == Qt::LeftButton && _boxSelection().isEnabled() && _boxSelection().state() == BoxSelection::STARTED )
  {

    _boxSelection().update( event->pos() );
    _synchronizeBoxSelection();
    emit copyAvailable( true );

    if( box_selection_timer_.isActive()) 
    {
      if( viewport()->rect().contains( event->pos() ) ) box_selection_timer_.stop();
    } else if (!viewport()->rect().contains( event->pos() )) box_selection_timer_.start(100, this);
    
    return;

  }

  // start a new box selection if requested
  if( event->buttons() == Qt::LeftButton && _boxSelection().isEnabled() && event->modifiers() == Qt::ControlModifier && viewport()->rect().contains( event->pos() ) )
  {

    _boxSelection().start( event->pos() );
    _synchronizeBoxSelection();
    emit copyAvailable( true );
    return;
  }

  // see if dragging existing box selection
  if( event->buttons() == Qt::LeftButton && _boxSelection().state() == BoxSelection::FINISHED && (event->pos() - drag_start_ ).manhattanLength() > QApplication::startDragDistance() )
  {
    // start drag
    QDrag *drag = new QDrag(this);

    // store data
    QString text( _boxSelection().toString() );

    QMimeData *data = new QMimeData();
    data->setText( text );
    data->setData( BoxSelection::mimeType, text.toAscii() );
    drag->setMimeData( data );
    drag->start(Qt::CopyAction);

    return;
  }

  return QTextEdit::mouseMoveEvent( event );

}

//________________________________________________
void TextEditor::mouseReleaseEvent( QMouseEvent* event )
{

  Debug::Throw( "TextEditor::mouseReleaseEvent.\n" );

  box_selection_timer_.stop();
  
  if( event->button() == Qt::MidButton ) 
  { Debug::Throw( "TextEditor::mouseReleaseEvent - middle mouse button.\n" ); }

  // no need to check for enability because there is no way for the box to start if disabled
  if( event->button() == Qt::LeftButton && _boxSelection().state() == BoxSelection::STARTED )
  {

    _boxSelection().finish( event->pos() );
    _synchronizeBoxSelection();
    return QTextEdit::mouseReleaseEvent( event );

  }

  if( event->button() == Qt::LeftButton && _boxSelection().state() == BoxSelection::FINISHED )
  {

    _boxSelection().clear();
    _synchronizeBoxSelection();
    emit copyAvailable( false );
    return QTextEdit::mouseReleaseEvent( event );

  }

  if( event->button() == Qt::LeftButton && click_counter_.counts() > 1 )
  {
    // when multiple-click is in progress
    // do nothing because it can reset the selection
    event->ignore();
    return;
  }

  if( event->button() == Qt::MidButton  && _boxSelection().state() == BoxSelection::FINISHED ) 
  {
    _boxSelection().clear();
    _boxSelection().clear();
  } 
  
  // process event
  QTextEdit::mouseReleaseEvent( event );

}

//________________________________________________
void TextEditor::dropEvent( QDropEvent* event )
{

  Debug::Throw( "TextEditor::dropEvent.\n" );

  // static empty mimeData used to pass to base class
  // so that drop events are finished properly even when actually doing nothing
  static QMimeData* empty_data( new QMimeData() );
  QDropEvent empty_event( event->pos(), event->possibleActions(), empty_data, Qt::NoButton, Qt::NoModifier );

  // if mimeData is block selection, block selection is enabled here
  // and there is no active selection (standard or box), insert new box selection
  // at cursor position
  if(
    event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
    _boxSelection().isEnabled() &&
    _boxSelection().state() == BoxSelection::EMPTY &&
    !textCursor().hasSelection() )
  {

    Debug::Throw( "TextEditor::dropEvent - dropping box selection.\n" );

    // retrieve text from mimeType
    QString text( event->mimeData()->text() );
    QStringList input_list( text.split( "\n" ) );

    // create an empty boxSelection from current position with proper size
    _boxSelection().start( event->pos() );
    _boxSelection().finish( event->pos() );
    _boxSelection().fromString( text );
    _boxSelection().clear();

    event->acceptProposedAction();
    QTextEdit::dropEvent( &empty_event );
    return;

  }

  if(
    event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
    _boxSelection().isEnabled() &&
    _boxSelection().state() == BoxSelection::FINISHED &&
    !toViewport( _boxSelection().rect() ).contains( event->pos() ) &&
    event->source() == this
    )
  {
    // drag is box selection and from this window. Move current block selection around.
    Debug::Throw( "TextEditor::dropEvent - [box] moving current box selection.\n" );

    // count rows in current selection
    int rowCount( _boxSelection().cursorList().size() - 1 );

    // store cursor at new insertion position
    QTextCursor new_cursor( cursorForPosition( event->pos() ) );

    // remove current selection
    _boxSelection().removeSelectedText();
    _boxSelection().clear();

    // prepare new selection
    QRect rect( cursorRect( new_cursor ) );
    QPoint start( rect.center().x(), rect.top() );
    QPoint end( rect.center().x(), rect.top() + rowCount*QFontMetrics( font() ).height() );

    _boxSelection().start( start );
    _boxSelection().finish( end );

    // join modifications with previous so that they appear as one entry in undo/redo list
    new_cursor.joinPreviousEditBlock();

    // insert text in new box
    _boxSelection().fromString( event->mimeData()->text() );
    _boxSelection().clear();
    new_cursor.endEditBlock();

    event->acceptProposedAction();
    QTextEdit::dropEvent( &empty_event );
    return;


  }

  // check if there is one valid box selection that contains the drop point
  if(
    event->mimeData()->hasText() &&
    _boxSelection().isEnabled() &&
    _boxSelection().state() == BoxSelection::FINISHED &&
    toViewport( _boxSelection().rect() ).contains( event->pos() ) )
  {

    if( event->source() == this )
    {

      // current selection is inserted in itself. Doing nothing
      Debug::Throw( "TextEditor::dropEvent - [box] doing nothing.\n" );
      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    } else {

      // insert mine data in current box selection
      Debug::Throw( "TextEditor::dropEvent - [box] inserting selection.\n" );
      _boxSelection().fromString( event->mimeData()->text() );
      setTextCursor( _boxSelection().cursorList().back() );
      _boxSelection().clear();
      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    }
  }

  // retrieve selection bounding rect
  if( event->mimeData()->hasText() && textCursor().hasSelection() )
  {
    QTextCursor cursor( textCursor() );
    QTextCursor new_cursor( cursorForPosition( event->pos() ) );

    bool contained(
      new_cursor.position() >= min( cursor.position(), cursor.anchor() ) &&
      new_cursor.position() <= max( cursor.position(), cursor.anchor() ) );

    if( contained && event->source() != this )
    {

      // drag action is from another widget and ends in selection. Replace this selection
      Debug::Throw( "TextEditor::dropEvent - inserting selection.\n" );
      cursor.insertText( event->mimeData()->text() );
      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    }

    if( event->source() == this )
    {

      // drag action is from this widget
      // insert selection at current location and remove old selection
      Debug::Throw( "TextEditor::dropEvent - moving selection.\n" );
      cursor.beginEditBlock();
      cursor.removeSelectedText();
      cursor.setPosition( new_cursor.position() );
      cursor.insertText( event->mimeData()->text() );
      cursor.endEditBlock();
      setTextCursor( cursor );

      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    }

  }

  // for all other cases, use default
  return QTextEdit::dropEvent( event );

}

//________________________________________________
void TextEditor::keyPressEvent( QKeyEvent* event )
{

  // clear line buffer.
  remove_line_buffer_.clear();

  /*
  need to grap Qt::CTRL+X, C and V event to forward them to the
  daughter implementation of cut, copy and paste, otherwise
  they are passed to the base class, with no way to override
  */
  if( event->modifiers() == Qt::ControlModifier )
  {
    if( event->key() == Qt::Key_X )
    {
      cut();
      event->ignore();
      return;
    }

    if( event->key() == Qt::Key_C )
    {
      copy();
      event->ignore();
      return;
    }

    if( event->key() == Qt::Key_V )
    {
      paste();
      event->ignore();
      return;
    }

  }

  // special key processing for box selection
  if( _boxSelection().state() == BoxSelection::FINISHED )
  {
    if(
      (event->key() >= Qt::Key_Shift &&  event->key() <= Qt::Key_ScrollLock) ||
      (event->key() >= Qt::Key_F1 &&  event->key() <= Qt::Key_F25) ||
      (event->key() >= Qt::Key_Super_L && event->key() <= Qt::Key_Direction_R ) ||
      (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier ) )
    { return QTextEdit::keyPressEvent( event ); }

    // if cursor move clear selection
    if( event->key() >= Qt::Key_Home && event->key() <= Qt::Key_Down )
    {
      _boxSelection().clear();
      return QTextEdit::keyPressEvent( event );
    }

    // if delete or backspace remove selection
    if( event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete )
    {
      _boxSelection().removeSelectedText();
      _boxSelection().clear();
      return;
    }

    // any other key should replace the selection
    if( event->key() == Qt::Key_Tab )
    {
      if( !_hasTabEmulation() ) _boxSelection().fromString( tabCharacter() );
      else
      {
        // retrieve position from begin of block
        int position( _boxSelection().cursorList().front().anchor() );
        position -= document()->findBlock( position ).position();
        int n( position % emulatedTabCharacter().size() );
        _boxSelection().fromString( emulatedTabCharacter().right( emulatedTabCharacter().size()-n ) );

      }

      _boxSelection().clear();

    } else if( !(event->text().isNull() || event->text().isEmpty() ) ) {
      
      _boxSelection().fromString( event->text() );
      _boxSelection().clear();
      
    }

    return;
  }

  // tab emulation
  if( event->key() == Qt::Key_Tab )
  {
    _insertTab();
    return;
  }

  // insertion mode
  if( event->key() == Qt::Key_Insert )
  {
    _toggleOverwriteMode();
    event->ignore();
    return;
  }

  // default event handling
  QTextEdit::keyPressEvent( event );

  return;
}

//_______________________________________________________
void TextEditor::focusInEvent( QFocusEvent* event )
{
  Debug::Throw() << "TextEditor::focusInEvent - " << key() << endl;
  emit hasFocus( this );
  QTextEdit::focusInEvent( event );
}

//________________________________________________
void TextEditor::contextMenuEvent( QContextMenuEvent* event )
{

  Debug::Throw( "TextEditor::contextMenuEvent.\n" );
  QMenu menu( this );
  installContextMenuActions( menu );
  menu.exec( event->globalPos() );

}

//______________________________________________________________
void TextEditor::resizeEvent( QResizeEvent* event )
{
  QTextEdit::resizeEvent( event );
  if( lineWrapMode() == QTextEdit::NoWrap ) return;
  if( event->oldSize().width() == event->size().width() ) return;
  if( !hasLineNumberDisplay() ) return;
  
  // tell line number display to update at next draw
  lineNumberDisplay().needUpdate();
  
}

//______________________________________________________________
void TextEditor::paintEvent( QPaintEvent* event )
{
  
  // handle block background
  QTextBlock first( cursorForPosition( event->rect().topLeft() ).block() );
  QTextBlock last( cursorForPosition( event->rect().bottomRight() ).block() );

  // create painter and translate from widget to viewport coordinates
  QPainter painter( viewport() );
  painter.setClipRect( event->rect() );
  
  painter.translate( -scrollbarPosition() );
  painter.setPen( Qt::NoPen );
  
  // loop over found blocks
  for( QTextBlock block( first ); block != last.next() && block.isValid(); block = block.next() )
  {

    // retrieve block data and check background
    // static cast is use because should be faster and safe enough here
    TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
    if( !(data && data->hasFlag( TextBlock::HAS_BACKGROUND|TextBlock::CURRENT_BLOCK ) ) ) continue;

    // retrieve block rect
    QRectF block_rect( document()->documentLayout()->blockBoundingRect( block ) );
    block_rect.setWidth( viewport()->width() + scrollbarPosition().x() );

    QColor color;
    if( data->hasFlag( TextBlock::CURRENT_BLOCK ) && blockHighlightAction().isEnabled() && blockHighlightAction().isChecked() )
    { color = highlight_color_; }

    if( data->hasFlag( TextBlock::HAS_BACKGROUND ) )
    { color = BASE::Color( color ).merge( data->background() ); }

    if( color.isValid() )
    {
      painter.setBrush( color );
      painter.drawRect( block_rect );
    }

  }

  if( _boxSelection().state() == BoxSelection::STARTED || _boxSelection().state() == BoxSelection::FINISHED )
  {    
    painter.setPen( _boxSelection().color() );
    painter.setBrush( _boxSelection().brush() );
    painter.drawRect( _boxSelection().rect() );
  }
  
  painter.end();
  
  // base class painting
  QTextEdit::paintEvent( event );
  
  return;

}

//______________________________________________________________
void TextEditor::timerEvent(QTimerEvent *event)
{
  
  if (event->timerId() == box_selection_timer_.timerId() ) 
  {
    const QPoint global_position = QCursor::pos();
    const QPoint position = viewport()->mapFromGlobal(global_position);
    QMouseEvent mouse_event(QEvent::MouseMove, position, global_position, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    mouseMoveEvent(&mouse_event);
  }

  return QTextEdit::timerEvent( event );
  
}

//______________________________________________________________
void TextEditor::scrollContentsBy( int dx, int dy )
{
  // mark margins dirty if vertical scroll is non empty
  if( dy != 0 ) _setMarginDirty();
  
  // base class call
  QTextEdit::scrollContentsBy( dx, dy );
  
}

//______________________________________________________________
void TextEditor::_installActions( void )
{
  Debug::Throw( "TextEditor::_installActions.\n" );

  // create actions
  addAction( undo_action_ = new QAction( IconEngine::get( ICONS::UNDO ), "&Undo", this ) );
  undo_action_->setShortcut( Qt::CTRL + Qt::Key_Z );
  undo_action_->setEnabled( document()->isUndoAvailable() );
  connect( undo_action_, SIGNAL( triggered() ), document(), SLOT( undo() ) );
  connect( this, SIGNAL( undoAvailable( bool ) ), undo_action_, SLOT( setEnabled( bool ) ) );

  addAction( redo_action_ = new QAction( IconEngine::get( ICONS::REDO ), "&Redo", this ) );
  redo_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_Z );
  redo_action_->setEnabled( document()->isRedoAvailable() );
  connect( redo_action_, SIGNAL( triggered() ), document(), SLOT( redo() ) );
  connect( this, SIGNAL( redoAvailable( bool ) ), redo_action_, SLOT( setEnabled( bool ) ) );

  addAction( cut_action_ = new QAction( IconEngine::get( ICONS::CUT ), "Cu&t", this ) );
  cut_action_->setShortcut( Qt::CTRL + Qt::Key_X );
  connect( cut_action_, SIGNAL( triggered() ), SLOT( cut() ) );

  addAction( copy_action_ = new QAction( IconEngine::get( ICONS::COPY ), "&Copy", this ) );
  copy_action_->setShortcut( Qt::CTRL + Qt::Key_C );
  connect( copy_action_, SIGNAL( triggered() ), SLOT( copy() ) );

  addAction( paste_action_ = new QAction( IconEngine::get( ICONS::PASTE ), "&Paste", this ) );
  paste_action_->setShortcut( Qt::CTRL + Qt::Key_V );
  connect( paste_action_, SIGNAL( triggered() ), SLOT( paste() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ), SLOT( _updatePasteAction() ) );
  _updatePasteAction();

  addAction( clear_action_ = new QAction( "&Clear", this ) );
  connect( clear_action_, SIGNAL( triggered() ), SLOT( clear() ) );

  addAction( select_all_action_ = new QAction( "Select &All", this ) );
  select_all_action_->setShortcut( Qt::CTRL + Qt::Key_A );
  select_all_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );

  addAction( upper_case_action_ = new QAction( "&Upper Case", this ) );
  upper_case_action_->setShortcut( Qt::CTRL + Qt::Key_U );
  upper_case_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( upper_case_action_, SIGNAL( triggered() ), SLOT( upperCase() ) );

  addAction( lower_case_action_ = new QAction( "&Lower Case", this ) );
  lower_case_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_U );
  lower_case_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( lower_case_action_, SIGNAL( triggered() ), SLOT( lowerCase() ) );

  addAction( find_action_ = new QAction( IconEngine::get( ICONS::FIND ), "&Find", this ) );
  find_action_->setShortcut( Qt::CTRL + Qt::Key_F );
  find_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_action_, SIGNAL( triggered() ), SLOT( _findFromDialog() ) );

  addAction( find_again_action_ = new QAction( "F&ind Again", this ) );
  find_again_action_->setShortcut( Qt::CTRL + Qt::Key_G );
  find_again_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_action_, SIGNAL( triggered() ), SLOT( findAgainForward() ) );

  addAction( find_again_backward_action_ = new QAction( this ) );
  find_again_backward_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_G );
  find_again_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_backward_action_, SIGNAL( triggered() ), SLOT( findAgainBackward() ) );

  addAction( find_selection_action_ = new QAction( "Find &Selection", this ) );
  find_selection_action_->setShortcut( Qt::CTRL + Qt::Key_H );
  find_selection_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_action_, SIGNAL( triggered() ), SLOT( findSelectionForward() ) );

  addAction( find_selection_backward_action_ = new QAction( this ) );
  find_selection_backward_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_H );
  find_selection_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_backward_action_, SIGNAL( triggered() ), SLOT( findSelectionBackward() ) );

  addAction( replace_action_ = new QAction( IconEngine::get( ICONS::FIND ), "Replace", this ) );
  replace_action_->setShortcut( Qt::CTRL + Qt::Key_R );
  replace_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_action_, SIGNAL( triggered() ), SLOT( _replaceFromDialog() ) );

  addAction( replace_again_action_ = new QAction( "Replace Again", this ) );
  replace_again_action_->setShortcut( Qt::CTRL + Qt::Key_T );
  replace_again_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_again_action_, SIGNAL( triggered() ), SLOT( replaceAgainForward() ) );

  addAction( replace_again_backward_action_ = new QAction( this ) );
  replace_again_backward_action_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_T );
  replace_again_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_again_backward_action_, SIGNAL( triggered() ), SLOT( replaceAgainBackward() ) );

  addAction( goto_line_action_ = new QAction( "&Goto Line Number", this ) );
  goto_line_action_->setShortcut( Qt::CTRL + Qt::Key_L );
  goto_line_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( goto_line_action_, SIGNAL( triggered() ), SLOT( _selectLineFromDialog() ) );

  // remove line action
  QAction* remove_line_action( new QAction( "&Remove Current Line", this ) );
  addAction( remove_line_action );
  remove_line_action->setShortcut( Qt::CTRL + Qt::Key_K );
  remove_line_action->setShortcutContext( Qt::WidgetShortcut );
  connect( remove_line_action, SIGNAL( triggered() ), SLOT( removeLine() ) );

  // current block highlight
  addAction( block_highlight_action_ = new QAction( "&Highlight Current Paragraph", this ) );
  block_highlight_action_->setCheckable( true );
  block_highlight_action_->setChecked( blockHighlight().isEnabled() );
  block_highlight_action_->setShortcut( Qt::Key_F12 );
  block_highlight_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( block_highlight_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleBlockHighlight( bool ) ) );

  // wrap mode
  addAction( wrap_mode_action_ = new QAction( "&Wrap Text", this ) );
  wrap_mode_action_->setCheckable( true );
  wrap_mode_action_->setChecked( lineWrapMode() == QTextEdit::WidgetWidth );
  wrap_mode_action_->setShortcut( Qt::Key_F10 );
  wrap_mode_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( wrap_mode_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleWrapMode( bool ) ) );

  // tab emulation action
  addAction( tab_emulation_action_ = new QAction( "&Emulate Tabs", this ) );
  tab_emulation_action_->setCheckable( true );
  tab_emulation_action_->setChecked( has_tab_emulation_ );
  connect( tab_emulation_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleTabEmulation( bool ) ) );
  
  // line number action
  addAction( show_line_number_action_ =new QAction( "Show Line Numbers", this ) );
  show_line_number_action_->setToolTip( "Show/hide line numbers" );
  show_line_number_action_->setCheckable( true );
  show_line_number_action_->setShortcut( Qt::Key_F11 );
  show_line_number_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( show_line_number_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleShowLineNumbers( bool ) ) );

  // update actions that depend on the presence of a selection
  _updateSelectionActions( textCursor().hasSelection() );

  #if QT_VERSION >= 0x040200
  // update actions that depend on the content of the clipboard
  // this is available only starting from Qt 4.2
  connect( qApp->clipboard(), SIGNAL( changed( QClipboard::Mode ) ), SLOT( _updateClipboardActions( QClipboard::Mode ) ) );
  #endif

}

//______________________________________________________________________
TextSelection TextEditor::selection( void ) const
{
  Debug::Throw( "TextEditor::selection.\n" );

  // copy last selection
  TextSelection out( "" );

  // copy attributes from last selection
  out.setFlag( TextSelection::CASE_SENSITIVE, lastSelection().flag( TextSelection::CASE_SENSITIVE ) );
  out.setFlag( TextSelection::ENTIRE_WORD, lastSelection().flag( TextSelection::ENTIRE_WORD ) );

  // try set from current selection
  QString text;
  if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) {
    
    Debug::Throw( "TextEditor::selection - from clipboard.\n" );
    out.setText( text );
  
  } else if( textCursor().hasSelection() ) {
    
    Debug::Throw() << "TextEditor::selection - from cursor: " << textCursor().selectedText() << endl;
    out.setText( textCursor().selectedText() );  
    
  } else {
  
    Debug::Throw( "TextEditor::selection - from last selection.\n" );   
    out.setText( lastSelection().text() );
    
  }
  
  return out;

}

//______________________________________________________________________
void TextEditor::_createBaseFindDialog( void )
{

  Debug::Throw( "TextEditor::_createBaseFindDialog.\n" );
  if( !find_dialog_ )
  {

    find_dialog_ = new BaseFindDialog( this );
    find_dialog_->setWindowTitle( "Find in Text" );
    connect( find_dialog_, SIGNAL( find( TextSelection ) ), SLOT( find( TextSelection ) ) );
    connect( this, SIGNAL( noMatchFound() ), find_dialog_, SLOT( noMatchFound() ) );
    connect( this, SIGNAL( matchFound() ), find_dialog_, SLOT( clearLabel() ) );

  }

  return;

}

//______________________________________________________________________
bool TextEditor::_findForward( const TextSelection& selection, const bool& rewind )
{
  Debug::Throw( "TextEditor::_findForward.\n" );
  if( selection.text().isEmpty() ) return false;

  // store selection
  setLastSelection( selection );

  // retrieve current cursor
  QTextCursor cursor( textCursor() );

  // if no_increment, start from the beginning of the possible current selection
  if( cursor.hasSelection() && selection.flag( TextSelection::NO_INCREMENT ) )
  { cursor.setPosition( cursor.anchor() ); }

  if( selection.flag( TextSelection::REGEXP ) )
  {

    // construct regexp and check
    QRegExp regexp( selection.text() );
    if( !regexp.isValid() )
    {
      InformationDialog( this, "Invalid regular expression. Find canceled" ).exec();
      return false;
    }

    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

    // make a copy of current cursor
    QTextCursor found( cursor );

    // if current text has selection that match, make sure pointer is located at the end of it
    if( found.hasSelection() && regexp.exactMatch( found.selectedText() ) )
    { found.setPosition( max( found.position(), found.anchor() ) ); }

    // move the found to the end of the document
    // and retrieve selected text
    found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
    QString text( found.selectedText() );

    // parse text
    int match = regexp.indexIn( text );
    int length = regexp.matchedLength();
    if( match < 0 )
    {
      // no match found
      // if not rewind, stop here
      if( !rewind ) return false;

      // update selection to the beginning of the document
      found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
      text = found.selectedText();
      match = regexp.indexIn( text );
      length = regexp.matchedLength();

    }

    // no match found. Return
    if( match < 0 ) return false;

    // match found. Update selection and return
    int position( match + min( found.anchor(), found.position() ) );
    found.setPosition( position, QTextCursor::MoveAnchor );
    found.setPosition( position+length, QTextCursor::KeepAnchor );
    setTextCursor( found );
    
    // copy selected text to clipboard
    if( qApp->clipboard()->supportsSelection() )
    { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }
    
    return true;

  } else {

    // search flags
    QTextDocument::FindFlags flags( 0 );
    if( selection.flag( TextSelection::CASE_SENSITIVE ) )  flags |= QTextDocument::FindCaseSensitively;
    if( selection.flag( TextSelection::ENTIRE_WORD ) ) flags |= QTextDocument::FindWholeWords;

    QTextCursor found( document()->find( selection.text(), cursor, flags ) );

    // find failed.
    if( found.isNull() && rewind )
    {
      cursor.movePosition( QTextCursor::Start );
      found = document()->find( selection.text(), cursor, flags );
    }

    if( found.isNull() ) return false;
    else {
      
      setTextCursor( found );

      // copy selected text to clipboard
      if( qApp->clipboard()->supportsSelection() )
      { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }
      
      return true;
    }

  }

  // useless
  return false;

}

//______________________________________________________________________
bool TextEditor::_findBackward( const TextSelection& selection, const bool& rewind )
{

  Debug::Throw( "TextEditor::_findBackward.\n" );
  if( selection.text().isEmpty() ) return false;
  setLastSelection( selection );

  // retrieve current cursor
  QTextCursor cursor( textCursor() );

  // if no_increment, start from the beginning of the possible current selection
  if( cursor.hasSelection() && selection.flag( TextSelection::NO_INCREMENT ) )
  { cursor.setPosition( cursor.anchor()+selection.text().size()+1 ); }
    if( selection.flag( TextSelection::REGEXP ) )
  {

    // construct regexp and check
    QRegExp regexp( selection.text() );
    if( !regexp.isValid() )
    {
      InformationDialog( this, "Invalid regular expression. Find canceled" ).exec();
      return false;
    }

    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

    // make a copy of current cursor
    QTextCursor found( cursor );

    // if current text has selection that match, make sure pointer is located at the end of it
    if( found.hasSelection() && regexp.exactMatch( found.selectedText() ) )
    { found.setPosition( min( found.position(), found.anchor() ) ); }

    // move cursor to beginning of the text
    found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
    QString text( found.selectedText() );

    // parse text
    int match = regexp.lastIndexIn( text );
    int length = regexp.matchedLength();
    if( match < 0 )
    {
      // no match found
      // if not rewind, stop here
      if( !rewind ) return false;

      // update selection to the beginning of the document
      found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
      text = found.selectedText();
      match = regexp.lastIndexIn( text );
      length = regexp.matchedLength();

    }

    // no match found. Return
    if( match < 0 ) return false;

    // match found. Update selection and return
    int position( match + min( found.anchor(), found.position() )+length );
    found.setPosition( position, QTextCursor::MoveAnchor );
    found.setPosition( position-length, QTextCursor::KeepAnchor );
    setTextCursor( found );

    // copy selected text to clipboard
    if( qApp->clipboard()->supportsSelection() )
    { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }
    
    return true;

  } else {

    // search flags
    QTextDocument::FindFlags flags( QTextDocument::FindBackward );
    if( selection.flag( TextSelection::CASE_SENSITIVE ) )  flags |= QTextDocument::FindCaseSensitively;
    if( selection.flag( TextSelection::ENTIRE_WORD ) ) flags |= QTextDocument::FindWholeWords;

    QTextCursor found( document()->find( selection.text(), cursor, flags ) );

    // find failed.
    if( found.isNull() && rewind )
    {
      cursor.movePosition( QTextCursor::End );
      found = document()->find( selection.text(), cursor, flags );
    }

    if( found.isNull() ) return false;
    else {
      setTextCursor( found );
      return true;
    }
  }

  // useless
  return false;

}

//______________________________________________________________________
void TextEditor::_createBaseReplaceDialog( void )
{

  Debug::Throw( "TextEditor::_createBaseReplaceDialog.\n" );
  if( !replace_dialog_ )
  {

    replace_dialog_ = new BaseReplaceDialog( this );
    replace_dialog_->setWindowTitle( "Replace in Text" );
    connect( replace_dialog_, SIGNAL( find( TextSelection ) ), SLOT( find( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replace( TextSelection ) ), SLOT( replace( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInWindow( TextSelection ) ), SLOT( replaceInWindow( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInSelection( TextSelection ) ), SLOT( replaceInSelection( TextSelection ) ) );
    connect( this, SIGNAL( noMatchFound() ), replace_dialog_, SLOT( noMatchFound() ) );
    connect( this, SIGNAL( matchFound() ), replace_dialog_, SLOT( clearLabel() ) );
    
  }

  Debug::Throw( "TextEditor::_createBaseReplaceDialog - done.\n" );
  return;

}


//__________________________________________________
void TextEditor::_createProgressDialog( void )
{
  
  Debug::Throw( "TextEditor::_createProgressDialog.\n" );
  
  // create dialog
  QProgressDialog* dialog = new QProgressDialog(0); 
  dialog->setAttribute( Qt::WA_DeleteOnClose, true );
  dialog->setLabelText( "Replace text in selection" );

  // connections
  connect( this, SIGNAL( busy( int ) ), dialog, SLOT( setMaximum( int ) ) );
  connect( this, SIGNAL( progressAvailable( int ) ), dialog, SLOT( setValue( int ) ) );
  connect( this, SIGNAL( idle( void ) ), dialog, SLOT( close( void ) ) );

  QtUtil::centerOnWidget( dialog, this ); 
  dialog->show();
  
}


//______________________________________________________________________
unsigned int TextEditor::_replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode )
{

  Debug::Throw()
    << "TextEditor::_replaceInRange -"
    << " anchor: " << cursor.anchor()
    << " position: " << cursor.position()
    << " selection: " << selection.text()
    << " replacement: " << selection.replaceText()
    << endl;

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return 0;

  // check selection
  if( selection.text().isEmpty() ) return 0;
  setLastSelection( selection );

  // check cursor
  if( !cursor.hasSelection() ) return 0;

  // store number of matches
  // and make local copy of cursor
  unsigned int found = 0;

  int saved_anchor( min( cursor.position(), cursor.anchor() ) );
  int saved_position( max( cursor.position(), cursor.anchor() ) );
  int current_position( saved_anchor );

  // check if regexp should be used or not
  if( selection.flag( TextSelection::REGEXP ) )
  {

    Debug::Throw( "TextEditor::_replaceInRange - regexp.\n" );

    // construct regexp and check
    QRegExp regexp( selection.text() );
    if( !regexp.isValid() )
    {
      InformationDialog( this, "Invalid regular expression. Find canceled" ).exec();
      return false;
    }

    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

    // replace everything in selected text
    QString selected_text( cursor.selectedText() );
    emit busy( selected_text.size() );
    
    for( int position = 0; (position = regexp.indexIn( selected_text, position )) != -1; )
    {
      // replace in selected text
      selected_text.replace( position, regexp.matchedLength(), selection.replaceText() );
      
      // replace in cursor
      /* this is to allow for undoing the changes one by one */
      cursor.setPosition( saved_anchor + position );
      cursor.setPosition( saved_anchor + position + regexp.matchedLength(), QTextCursor::KeepAnchor );
      cursor.insertText( selection.replaceText() );
      current_position = cursor.position();

      // increment position
      position += selection.replaceText().size();
      current_position = saved_anchor + position;

      found++;
      
      emit progressAvailable( position );

    }
    
    emit idle();

    // update cursor
    if( mode == EXPAND )
    {
      cursor.setPosition( saved_anchor );
      cursor.setPosition( saved_anchor + selected_text.length(), QTextCursor::KeepAnchor );

    } else if( mode == MOVE ) cursor.setPosition( current_position );

  } else {

    Debug::Throw( "TextEditor::_replaceInRange - normal replacement.\n" );

    emit busy( cursor.selectedText().size() );

    // changes local cursor to beginning of the selection
    cursor.setPosition( saved_anchor );

    // define search flags
    QTextDocument::FindFlags flags(0);
    if( selection.flag( TextSelection::CASE_SENSITIVE ) )  flags |= QTextDocument::FindCaseSensitively;
    if( selection.flag( TextSelection::ENTIRE_WORD ) ) flags |= QTextDocument::FindWholeWords;

    while( !( cursor = document()->find( selection.text(), cursor, flags ) ).isNull() && cursor.position() <= saved_position )
    {

      // perform replacement
      cursor.insertText( selection.replaceText() );
      current_position = cursor.position();
      saved_position += selection.replaceText().size() - selection.text().size();
      found ++;

      emit busy( saved_position );
      emit progressAvailable( current_position );

    }
    
    emit idle();

    if( mode == EXPAND )
    {
      cursor.setPosition( saved_anchor );
      cursor.setPosition( saved_position, QTextCursor::KeepAnchor );
    } else if( mode == MOVE ) cursor.setPosition( current_position );

  }

  Debug::Throw( "TextEditor::_replaceInRange - done.\n" );
  return found;

}

//_____________________________________________________________
void TextEditor::_synchronizeBoxSelection( void ) const
{

  if( !isSynchronized() ) return;

  // Debug::Throw( "TextEditor::_synchronizeBoxSelection.\n" );
  BASE::KeySet<TextEditor> displays( this );
  for( BASE::KeySet<TextEditor>::iterator iter = displays.begin(); iter != displays.end(); iter++ )
  { (*iter)->_boxSelection().synchronize( _boxSelection() ); }

}

//_____________________________________________________________
bool TextEditor::_setLeftMargin( const int& margin )
{

  Debug::Throw() << "TextEditor::_setLeftMargin - margin: " << margin << endl;
  if( margin == _leftMargin() ) return false;

  left_margin_ = margin;
  setViewportMargins( _leftMargin(), 0, 0, 0 );
  
  return true;

}

//_____________________________________________________________
QRect TextEditor::_marginRect( void ) const
{ return QRect( frameWidth(), frameWidth(), _leftMargin(), height()-2*frameWidth() ); }

//_____________________________________________________________
void TextEditor::_toggleOverwriteMode( void )
{
  Debug::Throw( "TextEditor::_toggleOverwriteMode.\n" );
  setOverwriteMode( !overwriteMode() );
  emit overwriteModeChanged();
  return;
}

//________________________________________________
bool TextEditor::_setTabSize( const int& tab_size )
{

  Debug::Throw() << "TextEditor::_setTabSize - " << tab_size << endl;
  assert( tab_size > 0 );

  int stop_width( tab_size * QFontMetrics( font() ).width( " " ) );
  if( tab_size == emulated_tab_.size() && tabStopWidth() == stop_width ) return false;

  // create strings and regular expressions
  // define normal tabs
  normal_tab_ = "\t";
  normal_tab_regexp_.setPattern( "^(\\t)+" );
  setTabStopWidth( stop_width );

  // define emulated tabs
  emulated_tab_ = QString( tab_size, ' ' );

  QString buffer;
  QTextStream( &buffer ) << "^(" << emulated_tab_ << ")" << "+";
  emulated_tab_regexp_.setPattern( buffer );

  return true;
}

//_____________________________________________________________
void TextEditor::_insertTab( void )
{
  Debug::Throw( "TextEditor::_insertTab.\n" );

  // retrieve current cursor
  QTextCursor cursor( textCursor() );
  if( !_hasTabEmulation() ) cursor.insertText( normalTabCharacter() );
  else {

    // retrieve position from begin of block
    int position( min( cursor.position(), cursor.anchor() ) );
    position -= document()->findBlock( position ).position();
    int n( position % emulatedTabCharacter().size() );
    cursor.insertText( emulatedTabCharacter().right( emulatedTabCharacter().size()-n ) );

  }

  return;

}

//___________________________________________________________________________
bool TextEditor::_updateMargins( void )
{

  Debug::Throw( "TextEditor::_updateMargins.\n" );
  int left_margin( 0 );
  
  if( showLineNumberAction().isChecked() && showLineNumberAction().isVisible() )
  { left_margin += lineNumberDisplay().width(); }
  
  return _setLeftMargin( left_margin );
  if( left_margin_ == left_margin ) return false;
  return true;
  
}

//___________________________________________________________________________
void TextEditor::_drawMargins( QPainter& painter )
{
       
  int height( TextEditor::height() - 2*frameWidth() );
  if( horizontalScrollBar()->isVisible() ) { height -= horizontalScrollBar()->height() + 2; }
  
  painter.setClipRect( painter.clipRegion().boundingRect().intersected( QRect( 0, 0, _leftMargin(), height ).translated( frameWidth(),  frameWidth() ) ) );
  painter.translate( frameWidth(),  frameWidth() );
  
  painter.setBrush( _marginBackgroundColor() );
  painter.setPen( Qt::NoPen );
  painter.drawRect( 0, 0, _leftMargin(), height );
  
  if( _drawVerticalLine() ) {
    painter.setBrush( QBrush( _marginForegroundColor(), Qt::Dense4Pattern ) );
    painter.drawRect( _leftMargin()-1, 0, 1, height ); 
    painter.setBrush( _marginBackgroundColor() );
  } 
  
  painter.setPen( _marginForegroundColor() );
  int y_offset = verticalScrollBar()->value();      
  painter.translate( 0, -y_offset );
  
  // draw lines
  if( 
    hasLineNumberDisplay() && 
    hasLineNumberAction() && 
    showLineNumberAction().isVisible() && 
    showLineNumberAction().isChecked() )
  { lineNumberDisplay().paint( painter ); }
 
}

//________________________________________________
void TextEditor::_updateConfiguration( void )
{

  Debug::Throw( "TextEditor::_updateConfiguration.\n" );

  // wrap mode
  if( wrapFromOptions() )
  { wrapModeAction().setChecked( XmlOptions::get().get<bool>( "WRAP_TEXT" ) ); }

  if( lineNumbersFromOptions() )
  { showLineNumberAction().setChecked( XmlOptions::get().get<bool>( "SHOW_LINE_NUMBERS" ) ); }

  // tab emulation
  _setTabSize( XmlOptions::get().get<int>("TAB_SIZE") );
  tabEmulationAction().setChecked( XmlOptions::get().get<bool>( "TAB_EMULATION" ) );

  // paragraph highlighting
  highlight_color_ = QColor( XmlOptions::get().raw( "HIGHLIGHT_COLOR" ) );
  blockHighlight().setEnabled( highlight_color_.isValid() && XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );
  blockHighlightAction().setEnabled( highlight_color_.isValid() );
  blockHighlightAction().setChecked( XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );

  // line numbers
  _setMarginForegroundColor( QColor( XmlOptions::get().raw("MARGIN_FOREGROUND") ) );
  _setMarginBackgroundColor( QColor( XmlOptions::get().raw("MARGIN_BACKGROUND") ) );
  _setDrawVerticalLine( XmlOptions::get().get<bool>( "MARGIN_VERTICAL_LINE" ) );
  
  // update margins
  lineNumberDisplay().updateWidth( document()->blockCount() );
  _updateMargins();
  
  // update box configuration
  // clear
  _boxSelection().updateConfiguration();
  if( !_boxSelection().isEnabled() && _boxSelection().state() != BoxSelection::EMPTY )
  {
    _boxSelection().clear();
    _synchronizeBoxSelection();
    emit copyAvailable( false );
  }


  return;

}

//________________________________________________
void TextEditor::_synchronizeSelection( void )
{

  //Debug::Throw( "TextEditor::_synchronizeSelection.\n" );
  if( !isSynchronized() ) return;

  BASE::KeySet<TextEditor> editors( this );
  for( BASE::KeySet<TextEditor>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
  {
    TextEditor &editor( **iter );

    // check if textCursor is different
    if(
      editor.textCursor().position() == textCursor().position() &&
      editor.textCursor().anchor() == textCursor().anchor() )
    continue;

    // store scrollbar positions
    QPoint scrollbars( editor.scrollbarPosition() );

    editor.setSynchronized( false );
    editor.setUpdatesEnabled( false );
    editor.setTextCursor( textCursor() );

    // restore scrollbar positions
    editor.horizontalScrollBar()->setValue( scrollbars.x() );
    editor.verticalScrollBar()->setValue( scrollbars.y() );

    editor.setUpdatesEnabled( true );
    editor.setSynchronized( true );
  }
}

//________________________________________________
void TextEditor::_updateReadOnlyActions( bool readonly )
{

  Debug::Throw( "TextEditor::_updateReadOnlyActions.\n" );
  bool has_selection( textCursor().hasSelection() );

  cut_action_->setEnabled( has_selection && !readonly );
  upper_case_action_->setEnabled( has_selection && !readonly );
  lower_case_action_->setEnabled( has_selection && !readonly );

  replace_action_->setEnabled( !readonly );
  replace_again_action_->setEnabled( !readonly );
  replace_again_backward_action_->setEnabled( !readonly );

}

//________________________________________________
void TextEditor::_updateSelectionActions( bool has_selection )
{

  Debug::Throw() << "TextEditor::_updateSelectionActions - has_selection: " << has_selection << endl;

  bool editable( !isReadOnly() );
  cut_action_->setEnabled( has_selection && editable );
  copy_action_->setEnabled( has_selection );
  upper_case_action_->setEnabled( has_selection && editable );
  lower_case_action_->setEnabled( has_selection && editable );

  #if QT_VERSION < 0x040200
  // update clipboard actions, based on the clipboard content
  // this is done only for QT versions < 4.2. For higher versions
  // this is called directly from a QClipboard signal
  _updateClipboardActions( QClipboard::Clipboard );
  _updateClipboardActions( QClipboard::Selection );
  #endif
}

//________________________________________________
void TextEditor::_updateClipboardActions( QClipboard::Mode mode )
{
  Debug::Throw( "TextEditor::_updateClipboardActions.\n" );

  if( mode == QClipboard::Clipboard )
  { paste_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Clipboard ).isEmpty() ); }

  if( mode == QClipboard::Selection )
  {
    Debug::Throw() << "TextEditor::_updateClipboardActions - clipboard: " << qApp->clipboard()->text( QClipboard::Selection ) << endl;
    find_selection_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
    find_selection_backward_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
  }

}

//________________________________________________
void TextEditor::_updateClipboard( void )
{
  Debug::Throw( "TextEditor::_updateClipboard.\n" );
  
  // copy selected text to clipboard
  if( qApp->clipboard()->supportsSelection() && textCursor().hasSelection() )
  { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }
  
}

//________________________________________________
void TextEditor::_updatePasteAction( void )
{

  Debug::Throw( "TextEditor::_updatePasteAction.\n" );
  bool editable( !isReadOnly() );
  bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
  paste_action_->setEnabled( editable && has_clipboard );

}

//_________________________________________________
void TextEditor::_toggleBlockHighlight( bool state )
{
  
  Debug::Throw( "TextEditor::_toggleBlockHighlight.\n" );
  
  // enable
  blockHighlight().setEnabled( highlight_color_.isValid() && state );
  
  // update options
  XmlOptions::get().set<bool>( "HIGHLIGHT_PARAGRAPH", state );
  
  // update current paragraph
  if( blockHighlight().isEnabled() ) blockHighlight().highlight();
  else blockHighlight().clear();

  // redraw
  lineNumberDisplay().setNeedCurrentBlockUpdate( true );
  update();
  
  // propagate to other displays
  if( isSynchronized() )
  {
    // temporarely disable synchronization
    // to avoid infinite loop
    setSynchronized( false );

    BASE::KeySet<TextEditor> displays( this );
    for( BASE::KeySet<TextEditor>::iterator iter = displays.begin(); iter != displays.end(); iter++ )

    { if( (*iter)->isSynchronized() ) (*iter)->blockHighlightAction().setChecked( state ); }
    setSynchronized( true );

  }

}

//________________________________________________
bool TextEditor::_toggleWrapMode( bool state )
{
  Debug::Throw() << "TextEditor::_toggleWrapMode - " << (state ? "True":"false") << endl;
  LineWrapMode mode( state ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
  if( mode == lineWrapMode() ) return false;

  setLineWrapMode( mode );

  // propagate to associated display
  if( isSynchronized() )
  {

    // temporarely disable synchronization
    // to avoid infinite loop
    setSynchronized( false );
    BASE::KeySet<TextEditor> editors( this );
    for( BASE::KeySet<TextEditor>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
    { if( (*iter)->isSynchronized() ) (*iter)->wrapModeAction().setChecked( state ); }
    setSynchronized( true );

  }

  return true;
}

//________________________________________________
bool TextEditor::_toggleTabEmulation( bool state )
{

  Debug::Throw() << "TextEditor::_toggleTabEmulation - " << (state ? "True":"false") << endl;

  // check if changed
  if( has_tab_emulation_ == state ) return false;

  // set flag
  has_tab_emulation_ = state;
  tab_ = has_tab_emulation_ ? emulated_tab_ : normal_tab_;
  tab_regexp_ = has_tab_emulation_ ? emulated_tab_regexp_ : normal_tab_regexp_;

  // propagate to associated display
  if( isSynchronized() )
  {

    // temporarely disable synchronization
    // to avoid infinite loop
    setSynchronized( false );
    BASE::KeySet<TextEditor> editors( this );
    for( BASE::KeySet<TextEditor>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
    { if( (*iter)->isSynchronized() ) (*iter)->tabEmulationAction().setChecked( state ); }
    setSynchronized( true );

  }

  return true;

}

//_______________________________________________________
void TextEditor::_toggleShowLineNumbers( bool state )
{

  _updateMargins();
  
  // update options
  XmlOptions::get().set<bool>( "SHOW_LINE_NUMBERS", state );
  
  // propagate to other displays
  if( isSynchronized() )
  {
    // temporarely disable synchronization
    // to avoid infinite loop
    setSynchronized( false );

    BASE::KeySet<TextEditor> displays( this );
    for( BASE::KeySet<TextEditor>::iterator iter = displays.begin(); iter != displays.end(); iter++ )
    { if( (*iter)->isSynchronized() ) (*iter)->showLineNumberAction().setChecked( state ); }
    setSynchronized( true );

  }

  return;
}

//________________________________________________________
void TextEditor::_blockCountChanged( int count )
{
  
  Debug::Throw( "TextEditor::_blockCountChanged.\n" );
  if( !( hasLineNumberDisplay() && lineNumberDisplay().updateWidth( count ) ) ) return;
  if( !( hasLineNumberAction() && showLineNumberAction().isChecked() && showLineNumberAction().isVisible() ) ) return;
  _updateMargins();
  update();
  
}

//_____________________________________________________________________
void TextEditor::_findFromDialog( void )
{
  Debug::Throw( "TextEditor::_findFromDialog.\n" );

  // set default text
  // update find text
  QString text( selection().text() );  
  if( !text.isEmpty() )
  {
    const int max_length( 1024 );
    text = text.left( max_length );
  }

  // create
  if( !find_dialog_ ) _createBaseFindDialog();
  _findDialog().enableRegExp( true );
  _findDialog().centerOnParent();
  _findDialog().show();
  _findDialog().synchronize();
  _findDialog().clearLabel();
  _findDialog().setText( text );
  
  // changes focus
  _findDialog().activateWindow();
  _findDialog().editor().setFocus();

  return;
}

//_____________________________________________________________________
void TextEditor::_replaceFromDialog( void )
{
  Debug::Throw( "TextEditor::_replaceFromDialog.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

    // create
  if( !replace_dialog_ ) _createBaseReplaceDialog();

  _replaceDialog().centerOnParent();
  _replaceDialog().show();

  /*
    setting the default text values
    must be done after the dialog is shown
    otherwise it may be automatically resized
    to very large sizes due to the input text
  */

  // synchronize combo-boxes
  _replaceDialog().synchronize();
  _replaceDialog().clearLabel();

  // update find text
  QString text;
  if( !( text = qApp->clipboard()->text( QClipboard::Selection) ).isEmpty() ) _replaceDialog().setText( text );
  else if( textCursor().hasSelection() ) _replaceDialog().setText( textCursor().selectedText() );
  else if( !( text = lastSelection().text() ).isEmpty() ) _replaceDialog().setText( text );

  // update replace text
  if( !lastSelection().replaceText().isEmpty() ) _replaceDialog().setReplaceText( lastSelection().replaceText() );

  // changes focus
  _replaceDialog().activateWindow();
  _replaceDialog().editor().setFocus();

  Debug::Throw( "TextEditor::_replaceFromDialog - done.\n" );
  return;
}

//________________________________________________
void TextEditor::_selectLineFromDialog( void )
{

  Debug::Throw( "TextEditor::_selectLineFromDialog.\n" );
  if( !select_line_dialog_ )
  {
    select_line_dialog_ = new SelectLineDialog( this );
    connect( select_line_dialog_, SIGNAL( lineSelected( int ) ), SLOT( selectLine( int ) ) );
  }

  select_line_dialog_->editor().clear();
  select_line_dialog_->centerOnParent();
  select_line_dialog_->show();
  select_line_dialog_->activateWindow();
  select_line_dialog_->editor().setFocus();

}

//________________________________________________
void TextEditor::_setMarginDirty( bool value )
{
 
  if( value == margin_dirty_ ) return;
  margin_dirty_ = value;
  if( value ) QFrame::update( _marginRect() );
  
}

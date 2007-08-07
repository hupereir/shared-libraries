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
  \file CustomTextEdit.cc
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QMenu>
#include <QClipboard>
#include <QRegExp>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextLayout>

#include "BaseIcons.h"
#include "CustomPixmap.h"
#include "CustomTextEdit.h"
#include "CustomTextDocument.h"
#include "FindDialog.h"
#include "IconEngine.h"
#include "ReplaceDialog.h"
#include "SelectLineDialog.h"
#include "TextBlockData.h"
#include "TextSeparator.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;
using namespace BASE;
using namespace Qt;

//______________________________________________
TextSelection CustomTextEdit::last_selection_;

//______________________________________________
CustomTextEdit::CustomTextEdit( QWidget *parent ):
  QTextEdit( parent ),
  Counter( "CustomTextEdit" ),
  find_dialog_( 0 ),
  replace_dialog_( 0 ),
  select_line_dialog_( 0 ),
  wrap_from_options_( true ),
  has_tab_emulation_( false ),
  highlight_enabled_( false ),
  highlight_available_( false ),
  synchronize_( false ),
  box_selection_( this ),
  remove_line_buffer_( this ),
  click_counter_( this )
{
  
  Debug::Throw( "CustomTextEdit::CustomTextEdit.\n" ); 

  // set customized document
  CustomTextDocument* document( new CustomTextDocument(0) );
  BASE::Key::associate( this, document );
  setDocument( document );
 
  // text highlight
  BaseTextHighlight* highlight = new BaseTextHighlight( document );
  BASE::Key::associate( document, highlight );
  highlight->setEnabled( false );
  
  // actions
  _installActions();
 
  // signal to make sure selection is synchronized between clones
  connect( this, SIGNAL( selectionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _highlightCurrentBlock() ) );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateConfiguration() ) );

  updateConfiguration();
   
}

//________________________________________________
CustomTextEdit::~CustomTextEdit( void )
{
  
  Debug::Throw( "CustomTextEdit::~CustomTextEdit.\n" );
  
  // cast document
  CustomTextDocument* document( dynamic_cast<CustomTextDocument*>( CustomTextEdit::document() ) );
  if( document && BASE::KeySet<CustomTextEdit>( document ).size() == 1 ) delete document;
  
  // update associates synchronization flags
  BASE::KeySet<CustomTextEdit> editors( this );

  // nothing to be done if no associates
  if( editors.empty() ) return;
  
  // keep position of current cursor
  int position( textCursor().position() );
  int anchor( textCursor().anchor() );
  
  // need to reset Text document
  // to avoid deletion while deleting this editor
  setDocument( new QTextDocument() );
  
  // keep reference to first associate
  CustomTextEdit &editor( **editors.begin() );
  
  // recreate an appropriate cursor
  QTextCursor cursor( editor.document() );
  cursor.setPosition( anchor );
  cursor.setPosition( position, QTextCursor::KeepAnchor );
  editor.setTextCursor( cursor );
  
  // turn off synchronization
  if( editors.size() == 1 ) editor.setSynchronized( false ); 

}

//________________________________________________
void CustomTextEdit::setHighlightEnabled( const bool& value )
{ 
  Debug::Throw() << "CustomTextEdit::setHighlightEnabled - value: " << (value ? "true":"false" ) << endl;
  
  highlight_enabled_ = value;
  
  if( highlight_enabled_ && highlight_available_ ) _highlightCurrentBlock(); 
  else _clearHighlightedBlock();

}
 
//________________________________________________
int CustomTextEdit::blockCount( void ) const
{ 
  Debug::Throw( "CustomTextEdit::blockCount.\n" );
  
  int count = 0;
  for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next(), count++ ) {}
  return count;
}
  
//________________________________________________
TextPosition CustomTextEdit::textPosition( void ) const
{
  
  Debug::Throw( "CustomTextEdit::textPosition.\n" );
  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );
  
  // calculate index
  TextPosition out;
  out.index() = cursor.position() - block.position(); 
  
  // rewind to begin of document to get paragraph index
  while( (block = block.previous()).isValid() ) out.paragraph()++;
  
  return out;
  
}

//________________________________________________
TextPosition CustomTextEdit::positionFromIndex( const int& index ) const
{
  Debug::Throw( "CustomTextEdit::positionFromIndex.\n" );
    
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
int CustomTextEdit::indexFromPosition( const TextPosition& position ) const
{
  
  Debug::Throw( "CustomTextEdit::indexFromPosition.\n" );

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
void CustomTextEdit::selectWord( void )
{
  Debug::Throw( "CustomTextEdit::selectWord.\n" );

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
    
    // see if current character is in base separator list
    while( begin > 0 &&  TextSeparator::get().base().find( text[begin-1] ) != TextSeparator::get().base().end() ) begin--;
    while( end < text.size() && TextSeparator::get().base().find( text[end] ) != TextSeparator::get().base().end() ) end++;
  
  } else if( TextSeparator::get().extended().find( text[begin] ) != TextSeparator::get().extended().end() ) {
    
    // see if current character is in extended separator list
    while( begin > 0 &&  TextSeparator::get().extended().find( text[begin-1] ) != TextSeparator::get().extended().end() ) begin--;
    while( end < text.size() && TextSeparator::get().extended().find( text[end] ) != TextSeparator::get().extended().end() ) end++;

  } else {
    
    // separator is in word
    while( begin > 0 &&  TextSeparator::get().all().find( text[begin-1] ) == TextSeparator::get().all().end() ) begin--;
    while( end < (int)text.size() && TextSeparator::get().all().find( text[end] ) == TextSeparator::get().all().end() ) end++;
    
  }
  
  // move cursor to begin of selection
  for( ;begin < local_position; local_position-- ) { cursor.movePosition( QTextCursor::Left, QTextCursor::MoveAnchor ); }
  for( ;local_position < end; local_position++ ) { cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor ); }
  
  // copy selected text to clipboard, in both text and HTML
  QMimeData *data( new QMimeData() );
  data->setText( cursor.selectedText() );
  data->setHtml( cursor.selectedText() );
  qApp->clipboard()->setMimeData( data, QClipboard::Selection );
  
  // qApp->clipboard()->setText( cursor.selectedText(), QClipboard::Selection );

  // assign cursor to Text editor
  setTextCursor( cursor );

  return;
  
}
  

//________________________________________________
void CustomTextEdit::selectLine( void )
{
  Debug::Throw( "CustomTextEdit::selectLine.\n" );
  
  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );
  cursor.setPosition( block.position() );
  cursor.setPosition( block.position() + block.length(), QTextCursor::KeepAnchor );
  
  // perform selection
  //cursor.select( QTextCursor::BlockUnderCursor );
  
  // copy selected text to clipboard, in both text and HTML
  QMimeData *data( new QMimeData() );
  data->setText( cursor.selectedText() );
  data->setHtml( cursor.selectedText() );
  qApp->clipboard()->setMimeData( data, QClipboard::Selection );
  
  // qApp->clipboard()->setText( cursor.selectedText(), QClipboard::Selection );

  // assign cursor to text editor and make sure it is visible
  setTextCursor( cursor );
  ensureCursorVisible();
  
  return;

}

//________________________________________________
void CustomTextEdit::synchronize( CustomTextEdit* editor )
{
  Debug::Throw( "CustomTextEdit::synchronize.\n" );
    
  // retrieve and cast old document
  CustomTextDocument* document( dynamic_cast<CustomTextDocument*>( QTextEdit::document() ) );  
  
  // assign new document and associate
  setDocument( editor->document() );
  BASE::Key::associate( this, dynamic_cast<CustomTextDocument*>( editor->document() ) );
 
  // delete old document, if needed
  if( document && BASE::KeySet<CustomTextEdit>( document ).size() == 1 ) delete document;
  
  // restore connections with document
  //connect( undo_action_, SIGNAL( triggered() ), CustomTextEdit::document(), SLOT( undo() ) );
  //connect( redo_action_, SIGNAL( triggered() ), CustomTextEdit::document(), SLOT( redo() ) );
  
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
  tabEmulationAction()->setChecked( editor->tabEmulationAction()->isChecked() );
  
  // synchronize wrap mode
  wrapModeAction()->setChecked( editor->wrapModeAction()->isChecked() );
  
  Debug::Throw( "CustomTextEdit::synchronize - done.\n" );

  return;
  
}

//__________________________________________________________________
void CustomTextEdit::showReplacements( const unsigned int& counts )
{
  
  Debug::Throw( "CustomTextEdit::showReplacements.\n" );
  
  ostringstream what;
  if( !counts ) what << "string not found.";
  else if( counts == 1 ) what << "1 replacement performed";
  else what << counts << " replacements performed";
  QtUtil::infoDialog( this, what.str() );
  return;
  
}

//________________________________________________
void CustomTextEdit::updateConfiguration( void )
{
  
  Debug::Throw( "CustomTextEdit::updateConfiguration.\n" );
 
  // wrap mode
  if( wrapFromOptions() )
  { wrapModeAction()->setChecked( XmlOptions::get().get<bool>( "WRAP_TEXT" ) ); }
  
  // tab emulation
  _setTabSize( XmlOptions::get().get<int>("TAB_SIZE") );
  tabEmulationAction()->setChecked( XmlOptions::get().get<bool>( "TAB_EMULATION" ) );
  
  // paragraph highlighting
  QColor highlight_color = QColor( XmlOptions::get().get<string>( "HIGHLIGHT_COLOR" ).c_str() );
  setHighlightEnabled( XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );  
  if( highlight_color.isValid() )
  {
    
    highlight_available_ = true;
    textHighlight().setHighlightColor( highlight_color );
    _highlightCurrentBlock();
    
  } else {
    
    highlight_available_ = false;
    _clearHighlightedBlock();
    
  }
  
  textHighlight().setEnabled( _highlightAvailable() && _highlightEnabled() );

  return;
  
}


//________________________________________________
void CustomTextEdit::upperCase( void )
{
  Debug::Throw( "CustomTextEdit::upperCase.\n" );
  QTextCursor cursor( textCursor() );
  
  // check selection
  if( !cursor.hasSelection() ) return;
  
  // retrieve selected text
  // convert to upperCase
  // insert in place of selection
  cursor.insertText( cursor.selectedText().toUpper() );
  return;
  
}

//________________________________________________
void CustomTextEdit::lowerCase( void )
{
  Debug::Throw( "CustomTextEdit::lowerCase.\n" );
  QTextCursor cursor( textCursor() );
  
  // check selection
  if( !cursor.hasSelection() ) return;
  
  // retrieve selected text
  // convert to lowercase
  // insert in place of selection
  cursor.insertText( cursor.selectedText().toLower() ); 
  return;
  
}

//_____________________________________________________________________
void CustomTextEdit::findFromDialog( void )
{
  Debug::Throw( "CustomTextEdit::findFromDialog.\n" );

  // create
  if( !find_dialog_ ) _createFindDialog();

  // enable/disable regexp
  _findDialog().enableRegExp( true );
  
  // raise dialog
  QtUtil::centerOnPointer( &_findDialog() );
  _findDialog().show();
   
  /* 
    setting the default text values 
    must be done after the dialog is shown
    otherwise it may be automatically resized 
    to very large sizes due to the input text
  */
  
  // set default string to find
  _findDialog().synchronize();
  _findDialog().clearLabel();

  // set default text
  if( textCursor().hasSelection() ) _findDialog().setText( textCursor().selectedText() );
  else if( !_lastSelection().text().isEmpty() ) _findDialog().setText( _lastSelection().text() );

  return;
}


//_____________________________________________________________________
void CustomTextEdit::replaceFromDialog( void )
{
  Debug::Throw( "CustomTextEdit::replaceFromDialog.\n" );

  // create
  if( !replace_dialog_ ) _createReplaceDialog();
     
  // enable/disable regexp
  _replaceDialog().enableRegExp( false );
  
  // raise dialog
  QtUtil::centerOnPointer( &_replaceDialog() );
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
  if( textCursor().hasSelection() ) _replaceDialog().setText( textCursor().selectedText() );
  else if( !_lastSelection().text().isEmpty() ) _replaceDialog().setText( _lastSelection().text() );

  // update replace text
  if( !_lastSelection().replaceText().isEmpty() ) _replaceDialog().setReplaceText( _lastSelection().replaceText() );

  return;
}

//______________________________________________________________________
void CustomTextEdit::replace( TextSelection selection )
{
 
  Debug::Throw( "CustomTextEdit::replace.\n" );
    
  // see if current selection match
  // perform replacement if yes
  QTextCursor cursor( textCursor() );
  if( 
    cursor.hasSelection() && 
    cursor.selectedText().size() == selection.text().size() && 
    cursor.selectedText().contains( 
      selection.text(), 
      selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) )
  {
    cursor.insertText( selection.replaceText() );
    setTextCursor( cursor );
  }
  
  // try find next occurence
  find( selection );
  return;
  
}

//______________________________________________________________________
unsigned int CustomTextEdit::replaceInSelection( TextSelection selection, const bool& show_dialog )
{
  
  Debug::Throw( "CustomTextEdit::replaceInSelection.\n" );
  unsigned int counts( _replaceInRange( selection, textCursor() ) );
  
  if( show_dialog ) showReplacements( counts );
  return counts;
  
}

//______________________________________________________________________
unsigned int CustomTextEdit::replaceInWindow( TextSelection selection, const bool& show_dialog )
{
  
  Debug::Throw( "CustomTextEdit::replaceInWindow.\n" );
  QTextCursor cursor( textCursor() );
  cursor.movePosition( QTextCursor::Start );
  cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
  unsigned int counts( _replaceInRange( selection, cursor ) );
  
  if( show_dialog ) showReplacements( counts );
  return counts;
  
}

//________________________________________________
void CustomTextEdit::selectLineFromDialog( void )
{
  
  Debug::Throw( "CustomTextEdit::selectLineFromDialog.\n" );
  if( !select_line_dialog_ ) 
  {
    select_line_dialog_ = new SelectLineDialog( this );
    connect( select_line_dialog_, SIGNAL( lineSelected( int ) ), SLOT( selectLine( int ) ) );
  }
  
  select_line_dialog_->editor().clear();
  QtUtil::centerOnPointer( select_line_dialog_ );
  select_line_dialog_->show();
  select_line_dialog_->editor().setFocus();
  
}

//________________________________________________
void CustomTextEdit::selectLine( int index )
{
  
  Debug::Throw() << "CustomTextEdit::selectLine - index: " << index << endl;
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
void CustomTextEdit::removeLine()
{
  
  Debug::Throw( "CustomTextEdit::removeLine.\n" );
  QTextCursor cursor( textCursor() );
  cursor.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
  cursor.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
  remove_line_buffer_.append( cursor.selectedText() );
  setTextCursor( cursor );
  cut();
  
}

//________________________________________________
void CustomTextEdit::mousePressEvent( QMouseEvent* event )
{

  Debug::Throw( "CustomTextEdit::mousePressEvent.\n" );
  
  // check button
  if( event->button() == LeftButton )
  {
    
    // increment multiple clicks
    QTextCursor cursor( textCursor() );
    switch( click_counter_.increment( cursorForPosition( event->pos() ).position() ) )
    {
      
      case 1:
      {
        if( event->modifiers() == Qt::ControlModifier ) 
        { 
          if( box_selection_.state() == BoxSelection::STARTED ) box_selection_.finish( event->pos() );
          if( box_selection_.state() == BoxSelection::FINISHED ) box_selection_.clear();
          box_selection_.start( event->pos() ); 
        } else {
          
          if( box_selection_.state() == BoxSelection::FINISHED ) box_selection_.clear();
          QTextEdit::mousePressEvent( event );
        
        }
        
      }
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
      break;
      
      default:
      event->ignore();
      break;
    }
    
  } else {
    
    if( box_selection_.state() == BoxSelection::FINISHED ) box_selection_.clear();
    QTextEdit::mousePressEvent( event );
  
  } return;
  
}

//________________________________________________
void CustomTextEdit::mouseDoubleClickEvent( QMouseEvent* event )
{
  
  Debug::Throw( "CustomTextEdit::mouseDoubleClickEvent.\n" );
  
  // check button
  if( event->button() == LeftButton ) mousePressEvent( event );
  else QTextEdit::mouseDoubleClickEvent( event );
  return;
  
}

//________________________________________________
void CustomTextEdit::mouseMoveEvent( QMouseEvent* event )
{

  Debug::Throw( "CustomTextEdit::mouseMoveEvent.\n" );
  if( event->button() == LeftButton && event->modifiers() == Qt::ControlModifier )
  {
    if( box_selection_.state() != BoxSelection::STARTED ) box_selection_.start(  event->pos() );
    else  box_selection_.update( event->pos() );
  } else return CustomTextEdit::mouseMoveEvent( event );
  
}

//________________________________________________
void CustomTextEdit::mouseReleaseEvent( QMouseEvent* event )
{
 
  Debug::Throw( "CustomTextEdit::mouseReleaseEvent.\n" );

  if( event->button() == LeftButton && box_selection_.state() == BoxSelection::STARTED )
  { box_selection_.finish( event->pos() ); }
  
  if( event->button() == LeftButton && click_counter_.counts() > 1 ) 
  { 
    // when multiple-click is in progress
    // do nothing because it can reset the selection
    event->ignore();
    return; 
  }
  
  QTextEdit::mouseReleaseEvent( event );
  return;
  
}
  

//________________________________________________
void CustomTextEdit::keyPressEvent( QKeyEvent* event )
{
  
  // clear line buffer.
  remove_line_buffer_.clear();
  
  // tab emulation
  if( event->key() == Key_Tab ) 
  {
    _insertTab();
    return;
  } 
  
  // insertion mode
  if( event->key() == Key_Insert ) 
  {
    _toggleInsertMode();
    event->ignore();
    return;
  } 
  
  // default event handling
  QTextEdit::keyPressEvent( event );
  
  return;
}

//________________________________________________
void CustomTextEdit::contextMenuEvent( QContextMenuEvent* event )
{
  
   Debug::Throw( "CustomTextEdit::contextMenuEvent.\n" );
    
  // menu
  QMenu menu( this );
  
  // wrapping
  menu.addAction( wrap_mode_action_ );
  menu.addSeparator();
  
  menu.addAction( undo_action_ );
  menu.addAction( redo_action_ );
  menu.addSeparator();

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
  menu.addAction( find_again_action_ ); 
  menu.addAction( find_selection_action_); 
  menu.addSeparator();
 
  menu.addAction( replace_action_ ); 
  menu.addAction( replace_again_action_ ); 
  menu.addAction( goto_line_action_);
 
  menu.exec( event->globalPos() );
  
}
 
//______________________________________________________________
void CustomTextEdit::_installActions( void )
{
  Debug::Throw( "CustomTextEdit::_installActions.\n" );
  
  // retrieve pixmaps path
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  
  // create actions
  addAction( undo_action_ = new QAction( IconEngine::get( ICONS::UNDO, path_list ), "&Undo", this ) );
  undo_action_->setShortcut( CTRL+Key_Z );
  undo_action_->setEnabled( document()->isUndoAvailable() );
  connect( undo_action_, SIGNAL( triggered() ), document(), SLOT( undo() ) );
  connect( this, SIGNAL( undoAvailable( bool ) ), undo_action_, SLOT( setEnabled( bool ) ) );

  addAction( redo_action_ = new QAction( IconEngine::get( ICONS::REDO, path_list ), "&Redo", this ) );
  redo_action_->setShortcut( SHIFT+CTRL+Key_Z );
  redo_action_->setEnabled( document()->isRedoAvailable() );
  connect( redo_action_, SIGNAL( triggered() ), document(), SLOT( redo() ) );
  connect( this, SIGNAL( redoAvailable( bool ) ), redo_action_, SLOT( setEnabled( bool ) ) );

  addAction( cut_action_ = new QAction( IconEngine::get( ICONS::CUT, path_list ), "Cu&t", this ) );
  cut_action_->setShortcut( CTRL+Key_X );
  connect( cut_action_, SIGNAL( triggered() ), SLOT( cut() ) );

  addAction( copy_action_ = new QAction( IconEngine::get( ICONS::COPY, path_list ), "&Copy", this ) );
  copy_action_->setShortcut( CTRL+Key_C );
  connect( copy_action_, SIGNAL( triggered() ), SLOT( copy() ) );

  addAction( paste_action_ = new QAction( IconEngine::get( ICONS::PASTE, path_list ), "&Paste", this ) );
  paste_action_->setShortcut( CTRL+Key_V );
  connect( paste_action_, SIGNAL( triggered() ), SLOT( paste() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ), SLOT( _updatePasteAction() ) );
  _updatePasteAction();
  
  addAction( clear_action_ = new QAction( "&Clear", this ) );
  connect( clear_action_, SIGNAL( triggered() ), SLOT( clear() ) );

  addAction( select_all_action_ = new QAction( "Select all", this ) );
  select_all_action_->setShortcut( CTRL+Key_A );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );
  
  addAction( upper_case_action_ = new QAction( "&Upper case", this ) );
  upper_case_action_->setShortcut( CTRL+Key_U );
  upper_case_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( upper_case_action_, SIGNAL( triggered() ), SLOT( upperCase() ) );

  addAction( lower_case_action_ = new QAction( "&Lower case", this ) );
  lower_case_action_->setShortcut( SHIFT+CTRL+Key_U );
  lower_case_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( lower_case_action_, SIGNAL( triggered() ), SLOT( lowerCase() ) );
  
  addAction( find_action_ = new QAction( IconEngine::get( ICONS::FIND, path_list ), "&Find", this ) );
  find_action_->setShortcut( CTRL+Key_F );
  find_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_action_, SIGNAL( triggered() ), SLOT( findFromDialog() ) );

  addAction( find_again_action_ = new QAction( "F&ind again", this ) );
  find_again_action_->setShortcut( CTRL+Key_G );
  find_again_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_action_, SIGNAL( triggered() ), SLOT( findAgainForward() ) );
 
  addAction( find_again_backward_action_ = new QAction( this ) );
  find_again_backward_action_->setShortcut( SHIFT+CTRL+Key_G );
  find_again_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_again_backward_action_, SIGNAL( triggered() ), SLOT( findAgainBackward() ) );

  addAction( find_selection_action_ = new QAction( "Find &selection", this ) );
  find_selection_action_->setShortcut( CTRL+Key_H );
  find_selection_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_action_, SIGNAL( triggered() ), SLOT( findSelectionForward() ) );
 
  addAction( find_selection_backward_action_ = new QAction( this ) );
  find_selection_backward_action_->setShortcut( SHIFT+CTRL+Key_H );
  find_selection_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( find_selection_backward_action_, SIGNAL( triggered() ), SLOT( findSelectionBackward() ) );

  addAction( replace_action_ = new QAction( IconEngine::get( ICONS::FIND, path_list ), "Replace", this ) );
  replace_action_->setShortcut( CTRL+Key_R );
  replace_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_action_, SIGNAL( triggered() ), SLOT( replaceFromDialog() ) );

  addAction( replace_again_action_ = new QAction( "Replace again", this ) );
  replace_again_action_->setShortcut( CTRL+Key_T );
  replace_again_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_again_action_, SIGNAL( triggered() ), SLOT( replaceAgainForward() ) );
 
  addAction( replace_again_backward_action_ = new QAction( this ) );
  replace_again_backward_action_->setShortcut( SHIFT+CTRL+Key_T );
  replace_again_backward_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( replace_again_backward_action_, SIGNAL( triggered() ), SLOT( replaceAgainBackward() ) );

  addAction( goto_line_action_ = new QAction( "&Goto line number", this ) );
  goto_line_action_->setShortcut( CTRL+Key_L );
  goto_line_action_->setShortcutContext( Qt::WidgetShortcut );
  connect( goto_line_action_, SIGNAL( triggered() ), SLOT( selectLineFromDialog() ) );
  
  // remove line action
  QAction* remove_line_action( new QAction( this ) ); 
  addAction( remove_line_action );
  remove_line_action->setShortcut( CTRL+Key_K );
  remove_line_action->setShortcutContext( Qt::WidgetShortcut );
  connect( remove_line_action, SIGNAL( triggered() ), SLOT( removeLine() ) );
  
  // wrap mode
  addAction( wrap_mode_action_ = new QAction( "&Wrap text", this ) );
  wrap_mode_action_->setCheckable( true );
  wrap_mode_action_->setChecked( lineWrapMode() == QTextEdit::WidgetWidth );
  connect( wrap_mode_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleWrapMode( bool ) ) );
  
  // tab emulation action
  addAction( tab_emulation_action_ = new QAction( "&Emulate tabs", this ) );
  tab_emulation_action_->setCheckable( true );
  tab_emulation_action_->setChecked( has_tab_emulation_ );
  connect( tab_emulation_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleTabEmulation( bool ) ) );
  
  // update actions that depend on the presence of a selection
  connect( this, SIGNAL( copyAvailable( bool ) ), SLOT( _updateSelectionActions( bool ) ) );
  _updateSelectionActions( textCursor().hasSelection() );
  
}

//______________________________________________________________________
TextSelection CustomTextEdit::_selection( void ) const
{
  Debug::Throw( "CustomTextEdit::_selection.\n" );
  
  // copy last selection
  TextSelection out( "" );
  
  // try set from current selection
  if( textCursor().hasSelection() ) out.setText( textCursor().selectedText() );
  else out.setText( qApp->clipboard()->text( QClipboard::Selection ) );
    
  // copy attributes from last selection
  out.setFlag( TextSelection::CASE_SENSITIVE, _lastSelection().flag( TextSelection::CASE_SENSITIVE ) );
  out.setFlag( TextSelection::ENTIRE_WORD, _lastSelection().flag( TextSelection::ENTIRE_WORD ) );
  return out;
  
}

//______________________________________________________________________
void CustomTextEdit::_createFindDialog( void )
{
  
  Debug::Throw( "CurstomTextEdit::_createFindDialog.\n" );
  if( !find_dialog_ )
  {
    
    find_dialog_ = new FindDialog( this );
    find_dialog_->polish();
    
    connect( find_dialog_, SIGNAL( find( TextSelection ) ), SLOT( find( TextSelection ) ) );
    connect( this, SIGNAL( noMatchFound() ), find_dialog_, SLOT( noMatchFound() ) );
    connect( this, SIGNAL( matchFound() ), find_dialog_, SLOT( clearLabel() ) );
    
  }
  
  return;
  
}

//______________________________________________________________________
bool CustomTextEdit::_findForward( const TextSelection& selection, const bool& rewind )
{
  Debug::Throw( "CustomTextEdit::_findForward.\n" );
  if( selection.text().isEmpty() ) return false;  
  
  // store selection
  _setLastSelection( selection );
  
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
      QtUtil::infoDialog( this, "invalid regular expression. Find canceled" );
      return false;
    }
    
    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive:Qt::CaseInsensitive );

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
      return true;
    }
    
  }
  
  // useless
  return false;
  
}

//______________________________________________________________________
bool CustomTextEdit::_findBackward( const TextSelection& selection, const bool& rewind )
{
  
  Debug::Throw( "CustomTextEdit::_findBackward.\n" );
  if( selection.text().isEmpty() ) return false;
  _setLastSelection( selection );
    
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
      QtUtil::infoDialog( this, "invalid regular expression. Find canceled" );
      return false;
    }
    
    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? Qt::CaseSensitive:Qt::CaseInsensitive );

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
void CustomTextEdit::_createReplaceDialog( void )
{
  
  Debug::Throw( "CurstomTextEdit::_createFindDialog.\n" );
  if( !replace_dialog_ )
  {
    
    replace_dialog_ = new ReplaceDialog( this );
    replace_dialog_->polish();
    
    connect( replace_dialog_, SIGNAL( find( TextSelection ) ), SLOT( find( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replace( TextSelection ) ), SLOT( replace( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInWindow( TextSelection ) ), SLOT( replaceInWindow( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInSelection( TextSelection ) ), SLOT( replaceInSelection( TextSelection ) ) );
    connect( this, SIGNAL( noMatchFound() ), replace_dialog_, SLOT( noMatchFound() ) );
    connect( this, SIGNAL( matchFound() ), replace_dialog_, SLOT( clearLabel() ) );
    
  }
  
  return;
  
}
  
//______________________________________________________________________
unsigned int CustomTextEdit::_replaceInRange( const TextSelection& selection, const QTextCursor& cursor )
{
  
  Debug::Throw() 
    << "CustomTextEdit::_replaceInRange -"
    << " anchor: " << cursor.anchor() 
    << " position: " << cursor.position() 
    << " selection: " << qPrintable( selection.text() )
    << " replacement: " << qPrintable( selection.replaceText() )
    << endl;
  
  // check selection
  if( selection.text().isEmpty() ) return 0;
  _setLastSelection( selection );
  
  // create cursor set at the beginning of the range
  QTextCursor local( cursor );
  local.setPosition( local.anchor() );
  int max_position( cursor.position() );
  
  // define search flags
  QTextDocument::FindFlags flags(0);
  if( selection.flag( TextSelection::CASE_SENSITIVE ) )  flags |= QTextDocument::FindCaseSensitively;
  if( selection.flag( TextSelection::ENTIRE_WORD ) ) flags |= QTextDocument::FindWholeWords;
  
  unsigned int found = 0;
  while( !( local = document()->find( selection.text(), local, flags ) ).isNull() && local.position() <= max_position )
  {
    
    // perform replacement
    local.insertText( selection.replaceText() );
    max_position += selection.replaceText().size() - selection.text().size();
    found ++;

  }
    
  return found;
  
}

//_____________________________________________________________
void CustomTextEdit::_clearHighlightedBlock( void )
{
  Debug::Throw( "CustomTextEdit::_clearHighlightedBlock.\n" );
  
  // loop over all blocks
  for( QTextBlock block = document()->begin(); block.isValid(); block = block.next() )
  {
    
    if( textCursor().block() == block && !( _highlightEnabled() && _highlightAvailable() ) ) continue;
    
    TextBlockData* data( dynamic_cast<TextBlockData*>( block.userData() ) );
    if( data && data->isCurrentBlock() ) 
    {
      data->setIsCurrentBlock( false );
      document()->markContentsDirty(block.position(), block.length());
    }
    
  }
    
  return;
  
}

//_____________________________________________________________
void CustomTextEdit::_toggleInsertMode( void )
{
  Debug::Throw( "CustomTextEdit::_toggleInsertMode.\n" );

  bool mode( overwriteMode() );
  mode = !mode;
  setOverwriteMode( mode );

  return;

}
 
//________________________________________________
bool CustomTextEdit::_setTabSize( const int& tab_size )
{
  
  Debug::Throw() << "CustomTextEdit::_setTabSize - " << tab_size << endl;
  Exception::check( tab_size > 0, DESCRIPTION( "invalid tab size" ) );
  
  if( tab_size == emulated_tab_.size() )
  {
    Debug::Throw() << "CustomTextEdit::_setTabSize - unchanged." << endl;
    return false;
  }

  // create strings and regular expressions
  // define normal tabs
  normal_tab_ = "\t";
  normal_tab_regexp_.setPattern( "^(\\t)+" );
  setTabStopWidth( tab_size*fontMetrics().width(" ") );
  
  // define emulated tabs
  emulated_tab_ = QString( tab_size, ' ' );
 
  ostringstream what;
  what << "^(" << qPrintable( emulated_tab_ ) << ")" << "+";
  emulated_tab_regexp_.setPattern( what.str().c_str() );
  
  return true;
}
 
//_____________________________________________________________
void CustomTextEdit::_insertTab( void )
{
  Debug::Throw( "CustomTextEdit::_insertTab.\n" );
  
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

//________________________________________________
void CustomTextEdit::_highlightCurrentBlock( void )
{ 
  Debug::Throw( "CustomTextEdit::_highlightCurrentBlock.\n" );
  
  // check if highlight is available and enabled
  if( !( _highlightEnabled() && _highlightAvailable() ) ) return;

  // retrieve current block
  QTextBlock block( textCursor().block() );  
  
  TextBlockData* data = dynamic_cast<TextBlockData*>( block.userData() );
  if( !data )
  {
    data = new TextBlockData();
    block.setUserData( data );
  } else if( data->isCurrentBlock() ) return;
  
  _clearHighlightedBlock();
  data->setIsCurrentBlock( true );
  document()->markContentsDirty(block.position(), block.length());
  
  return; 
}
  
//________________________________________________
void CustomTextEdit::_synchronizeSelection( void )
{
 
  Debug::Throw( "CustomTextEdit::_synchronizeSelection.\n" );
  if( !isSynchronized() ) return;
    
  BASE::KeySet<CustomTextEdit> editors( this );
  for( BASE::KeySet<CustomTextEdit>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
  {
    CustomTextEdit &editor( **iter );
    
    // check if textCursor is different
    if( 
      editor.textCursor().position() == textCursor().position() &&
      editor.textCursor().anchor() == textCursor().anchor() ) 
    continue;
    
    // store scrollbar positions
    int x( editor.horizontalScrollBar()->value() );
    int y( editor.verticalScrollBar()->value() );
    
    editor.setSynchronized( false );
    editor.setUpdatesEnabled( false );
    editor.setTextCursor( textCursor() );
    
    // restore scrollbar positions
    editor.horizontalScrollBar()->setValue( x );
    editor.verticalScrollBar()->setValue( y );
    
    editor.setUpdatesEnabled( true );
    editor.setSynchronized( true );
  }
}
 
//________________________________________________
void CustomTextEdit::_updateReadOnlyActions( bool readonly )
{
  
  Debug::Throw( "CustomTextEdit::_updateReadOnlyActions.\n" );
  bool has_selection( textCursor().hasSelection() );
  
  cut_action_->setEnabled( has_selection && !readonly );
  upper_case_action_->setEnabled( has_selection && !readonly );
  lower_case_action_->setEnabled( has_selection && !readonly );
  
  replace_action_->setEnabled( !readonly );
  replace_again_action_->setEnabled( !readonly );
  replace_again_backward_action_->setEnabled( !readonly );

}

//________________________________________________
void CustomTextEdit::_updateSelectionActions( bool has_selection )
{
  
  Debug::Throw( "CustomTextEdit::_updateSelectionActions.\n" );

  bool editable( !isReadOnly() );
  cut_action_->setEnabled( has_selection && editable );
  copy_action_->setEnabled( has_selection );
  upper_case_action_->setEnabled( has_selection && editable );
  lower_case_action_->setEnabled( has_selection && editable );
  find_selection_action_->setEnabled( has_selection );
  find_selection_backward_action_->setEnabled( has_selection );

}

//________________________________________________
void CustomTextEdit::_updatePasteAction( void )
{
  
  Debug::Throw( "CustomTextEdit::_updatePasteAction.\n" );
  bool editable( !isReadOnly() );
  bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
  paste_action_->setEnabled( editable && has_clipboard );
  
}
  

//________________________________________________
bool CustomTextEdit::_toggleWrapMode( bool state )
{
  Debug::Throw() << "CustomTextEdit::_toggleWrapMode - " << (state ? "true":"false") << endl;
  LineWrapMode mode( state ? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
  if( mode == lineWrapMode() ) return false;
  
  setLineWrapMode( mode );
  
  // propagate to associated display
  if( isSynchronized() )
  {
    
    // temporarely disable synchronization 
    // to avoid infinite loop
    setSynchronized( false );
    BASE::KeySet<CustomTextEdit> editors( this );
    for( BASE::KeySet<CustomTextEdit>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
    { if( (*iter)->isSynchronized() ) (*iter)->wrapModeAction()->setChecked( state ); }
    setSynchronized( true );
    
  }
  
  return true;
}

//________________________________________________
bool CustomTextEdit::_toggleTabEmulation( bool state )
{
  
  Debug::Throw() << "CustomTextEdit::_toggleTabEmulation - " << (state ? "true":"false") << endl;
 
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
    BASE::KeySet<CustomTextEdit> editors( this );
    for( BASE::KeySet<CustomTextEdit>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
    { if( (*iter)->isSynchronized() ) (*iter)->tabEmulationAction()->setChecked( state ); }
    setSynchronized( true );
    
  }
  
  return true;
  
}

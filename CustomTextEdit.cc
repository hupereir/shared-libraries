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
#include <QScrollBar>
#include <QTextBlock>

#include "CustomTextEdit.h"
#include "FindDialog.h"
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
  synchronize_( false ),
  highlight_enabled_( false ),
  highlight_available_( false ),
  click_counter_( this )
{
  
  Debug::Throw( "CustomTextEdit::CustomTextEdit.\n" ); 
  
  _installShortcuts(); 
  
  // signal to make sure selection is synchronized between clones
  connect( this, SIGNAL( selectionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _highlightCurrentBlock() ) );
  
  connect( qApp, SIGNAL( configurationChanged() ), this, SLOT( updateConfiguration() ) );

}

//________________________________________________
CustomTextEdit::~CustomTextEdit( void )
{
  Debug::Throw( "CustomTextEdit::~CustomTextEdit.\n" );  
}

//________________________________________________
void CustomTextEdit::enableShortCuts( const bool& value )
{ 
  Debug::Throw() << "CustomTextEdit::enableShortCuts - value: " << (value ? "true":"false" ) << endl;
  for( std::vector<QShortcut*>::iterator iter = shortcuts_.begin(); iter != shortcuts_.end(); iter++ )
  { (*iter)->setEnabled( value ); }
}  

//________________________________________________
void CustomTextEdit::enableBlockHighlight( const bool& value )
{ 
  Debug::Throw() << "CustomTextEdit::enableBlockHighlight - value: " << (value ? "true":"false" ) << endl;
  
  highlight_enabled_ = value;
  
  if( highlight_enabled_ && highlight_available_ ) _highlightCurrentBlock(); 
  else _clearHighlightedBlock();
  
}
  
//________________________________________________
TextPosition CustomTextEdit::textPosition( void ) const
{
  
  Debug::Throw( "CustomTextEdit::textPosition" );
  QTextCursor cursor( textCursor() );
  QTextBlock block( cursor.block() );
  
  // calculate index
  int index = cursor.position() - block.position(); 
  
  // calculate paragraph
  // this is a kludge, because there is no way to access it directly 
  // for this version of Qt
  int paragraph( 0 );
  while( (block = block.previous()).isValid() ) paragraph++;
  
  return TextPosition( paragraph, index );
  
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
  setTextCursor( cursor );

  return;
  
}
  

//________________________________________________
void CustomTextEdit::selectLine( void )
{
  Debug::Throw( "CustomTextEdit::selectLine.\n" );
  
  QTextCursor cursor( textCursor() );
  cursor.select( QTextCursor::BlockUnderCursor );
  setTextCursor( cursor );
  ensureCursorVisible();
  
  return;

}

//________________________________________________
void CustomTextEdit::synchronize( CustomTextEdit& editor )
{
  Debug::Throw( "CustomTextEdit::clone.\n" );
    
  setDocument( editor.document() );
  
  editor.setSynchronize( true );
  setSynchronize( true );
    
  // associate with this editor and associates
  BASE::KeySet<CustomTextEdit> editors( editor );
  editors.insert( &editor );
  
  // Key association
  for( BASE::KeySet<CustomTextEdit>::iterator iter = editors.begin(); iter != editors.end(); iter++ )
  { BASE::Key::associate( *iter, this ); }
  
  // selection synchronization
  _synchronizeSelection();

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
  QtUtil::infoDialogExclusive( this, what.str() );
  return;
  
}

//________________________________________________
void CustomTextEdit::updateConfiguration( void )
{
  
  Debug::Throw( "CustomTextEdit::updateConfiguration.\n" );
  QColor highlight_color;
  
  highlight_color = QColor( XmlOptions::get().get<string>( "HIGHLIGHT_COLOR" ).c_str() );
  
  if( highlight_color.isValid() )
  {
    highlight_available_ = true;
    
    highlight_format_ = QTextBlockFormat();
    highlight_format_.setBackground( highlight_color );
    
    default_format_ = QTextBlockFormat();
    default_format_.setBackground( palette().color( QPalette::Base ) );
    
    if( highlight_enabled_ ) _highlightCurrentBlock();
  } else {
    
    highlight_available_ = false;
    _clearHighlightedBlock();
    
  }
  
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
  // convert to upperCase
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
   
  if( textCursor().hasSelection() ) 
  {
    Debug::Throw() << "CustomTextEdit::findFromDialog - selection: " << qPrintable( textCursor().selectedText() ) << endl;
    _findDialog().setText( textCursor().selectedText() );
  } else if( !_lastSelection().text().isEmpty() ) _findDialog().setText( _lastSelection().text() );

  // set default string to find
  _findDialog().synchronize();
  _findDialog().clearLabel();

  // enable/disable regexp
  _findDialog().enableRegExp( false );
  
  // raise dialog
  QtUtil::centerOnPointer( &_findDialog() );
  _findDialog().show();
  
  return;
}


//_____________________________________________________________________
void CustomTextEdit::replaceFromDialog( void )
{
  Debug::Throw( "CustomTextEdit::replaceFromDialog.\n" );

  // create
  if( !replace_dialog_ ) _createReplaceDialog();
 
  // update find text
  if( textCursor().hasSelection() ) _replaceDialog().setText( textCursor().selectedText() );
  else if( !_lastSelection().text().isEmpty() ) _replaceDialog().setText( _lastSelection().text() );
 
  // set default string to find
  _replaceDialog().synchronize();
  _replaceDialog().clearLabel();
 
  // update replace text
  if( !_lastSelection().replaceText().isEmpty() ) _replaceDialog().setReplaceText( _lastSelection().replaceText() );
  
  // enable/disable regexp
  _replaceDialog().enableRegExp( false );
  
  // raise dialog
  QtUtil::centerOnPointer( &_replaceDialog() );
  _replaceDialog().show();
  
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
    connect( select_line_dialog_, SIGNAL( lineSelected( int ) ), this, SLOT( selectLine( int ) ) );
  }
  
  select_line_dialog_->show();
  
}

//________________________________________________
void CustomTextEdit::selectLine( int index )
{
   Debug::Throw() << "CustomTextEdit::selectLine - index: " << index << endl;
  int local_index( 0 );
  QTextBlock block = document()->begin();
  for( ;local_index <= index && block.isValid(); block = block.next(), local_index++ )
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
  setTextCursor( cursor );
  cut();
  
}

//________________________________________________
void CustomTextEdit::_highlightCurrentBlock( void )
{
  Debug::Throw( "CustomTextEdit::_highlightCurrentBlock.\n" );
  
  // check if highlight is available and enabled
  if( !( highlight_enabled_ && highlight_available_ ) ) return;

  // retrieve current block
  QTextCursor cursor( textCursor().block() );
  QTextBlock block( textCursor().block() );
  
  // try retrieve data, create if not found
  TextBlockData* data( static_cast<TextBlockData*>( block.userData() ) );
  if( !data ) block.setUserData( data = new TextBlockData() );
  
  // check if current block is not already active
  if( data->isCurrentBlock() ) return;
  
  // clear last highlighted block
  _clearHighlightedBlock();
  
  // set block as active
  data->setCurrentBlock( true );
  cursor.joinPreviousEditBlock();
  cursor.setBlockFormat( highlight_format_ );
  cursor.endEditBlock();
  
  return;
}
  
//________________________________________________
void CustomTextEdit::_synchronizeSelection( void )
{
 
  Debug::Throw( "CustomTextEdit::_synchronizeSelection.\n" );
  if( !synchronize() ) return;
    
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
    
    editor.setSynchronize( false );
    editor.setUpdatesEnabled( false );
    editor.setTextCursor( textCursor() );
    
    // restore scrollbar positions
    editor.horizontalScrollBar()->setValue( x );
    editor.verticalScrollBar()->setValue( y );
    
    editor.setUpdatesEnabled( true );
    editor.setSynchronize( true );
  }
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
      QTextEdit::mousePressEvent( event );
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
    
  } else QTextEdit::mousePressEvent( event );
  return;
  
}

//________________________________________________
void CustomTextEdit::mouseReleaseEvent( QMouseEvent* event )
{
 
  Debug::Throw( "CustomTextEdit::mouseReleaseEvent.\n" );
  
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
void CustomTextEdit::mouseDoubleClickEvent( QMouseEvent* event )
{
  
  Debug::Throw( "CustomTextEdit::mouseDoubleClickEvent.\n" );
  
  // check button
  if( event->button() == LeftButton ) mousePressEvent( event );
  else QTextEdit::mouseDoubleClickEvent( event );
  return;
  
}

//________________________________________________
void CustomTextEdit::keyPressEvent( QKeyEvent* event )
{
  Debug::Throw( "CustomTextEdit::KeyPressEvent.\n" );
  
  if( event->key() == Key_Insert ) 
  {
    _toggleInsertMode();
    event->ignore();
    return;
  }
  
  QTextEdit::keyPressEvent( event );
  return;
}

//________________________________________________
void CustomTextEdit::contextMenuEvent( QContextMenuEvent* event )
{
  
   Debug::Throw( "CustomTextEdit::contextMenuEvent.\n" );
  
  // retrieve flags
  bool editable( !isReadOnly() );
  bool has_selection( textCursor().hasSelection() );
  
  // retrieve document
  QTextDocument& document( *CustomTextEdit::document() );
  
  // menu
  QMenu menu( this );
  menu.addAction( "&Undo", &document, SLOT( undo() ), CTRL+Key_Z )->setEnabled( editable && document.isUndoAvailable() );
  menu.addAction( "&Redo", &document, SLOT( redo() ), SHIFT+CTRL+Key_Z )->setEnabled( editable && document.isRedoAvailable() );
  menu.addSeparator();

  menu.addAction( "Cu&t", this , SLOT( cut() ), CTRL+Key_X )->setEnabled( editable && has_selection );
  menu.addAction( "&Copy", this , SLOT( copy() ), CTRL+Key_C )->setEnabled( has_selection );
  menu.addAction( "&Paste", this , SLOT( paste() ), CTRL+Key_V )->setEnabled( editable );
  menu.addAction( "Clear", this , SLOT( clear() ) )->setEnabled( editable );
  menu.addSeparator();
 
  menu.addAction( "Select all", this , SLOT( selectAll() ), CTRL+Key_A ); 
  menu.addAction( "&Upper case", this, SLOT( upperCase( void ) ), CTRL+Key_U )->setEnabled( editable && has_selection );
  menu.addAction( "&Lower case", this, SLOT( lowerCase( void ) ), SHIFT+CTRL+Key_U )->setEnabled( editable && has_selection );
  menu.addSeparator();

  menu.addAction( "&Find", this , SLOT( findFromDialog() ), CTRL+Key_F ); 
  menu.addAction( "Find &again", this , SLOT( findAgainForward() ), CTRL+Key_G ); 
  menu.addAction( "Find &selection", this , SLOT( findSelectionForward() ), CTRL+Key_H ); 
  menu.addSeparator();
 
  menu.addAction( "&Replace", this , SLOT( replaceFromDialog() ), CTRL+Key_R ); 
  menu.addAction( "Replace a&gain", this , SLOT( replaceAgainForward() ), CTRL+Key_T ); 
  menu.addAction( "Goto line number", this, SLOT( selectLineFromDialog() ), CTRL+Key_L );
 
  menu.exec( event->globalPos() );
  
}

//______________________________________________________________________
TextSelection CustomTextEdit::_selection( void ) const
{
  Debug::Throw( "CustomTextEdit::_selection.\n" );
  
  // copy last selection
  TextSelection out( "" );
  
  // try set from current selection
  if( textCursor().hasSelection() ) out.setText( textCursor().selectedText() );
  else {
    
    // try retrieve from clipboard
    out.setText( qApp->clipboard()->text( QClipboard::Selection ) );
    
  } 
  
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
    
    connect( find_dialog_, SIGNAL( find( TextSelection ) ), this, SLOT( find( TextSelection ) ) );
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
    
  setTextCursor( found );
  return true;
  
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
  
  setTextCursor( found );
  return true;
  
}

//______________________________________________________________________
void CustomTextEdit::_createReplaceDialog( void )
{
  
  Debug::Throw( "CurstomTextEdit::_createFindDialog.\n" );
  if( !replace_dialog_ )
  {
    
    replace_dialog_ = new ReplaceDialog( this );
    replace_dialog_->polish();
    
    connect( replace_dialog_, SIGNAL( find( TextSelection ) ), this, SLOT( find( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replace( TextSelection ) ), this, SLOT( replace( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInWindow( TextSelection ) ), this, SLOT( replaceInWindow( TextSelection ) ) );
    connect( replace_dialog_, SIGNAL( replaceInSelection( TextSelection ) ), this, SLOT( replaceInSelection( TextSelection ) ) );
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
    
    // try retrieve data
    // check if block is current
    TextBlockData* data( static_cast<TextBlockData*>( block.userData() ) );
    if( !( data && data->isCurrentBlock() )  ) continue;
    
    // reset data
    data->setCurrentBlock( false );
    
    // reset textBlockFormat
    QTextCursor cursor( block );
    cursor.joinPreviousEditBlock();
    cursor.setBlockFormat( default_format_ );
    cursor.endEditBlock();
    
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

//______________________________________________________________________
void CustomTextEdit::_installShortcuts( void )
{
 Debug::Throw( "CustomTextEdit::_installShortCuts.\n" );
  
  // default shortcuts
  _addShortcut( CTRL+Key_Z, document(), SLOT( undo() ) );
  _addShortcut( SHIFT+CTRL+Key_Z, document(), SLOT( redo() ) );
  _addShortcut( CTRL+Key_X, this, SLOT( cut() ) );
  _addShortcut( CTRL+Key_C, this, SLOT( copy() ) );
  _addShortcut( CTRL+Key_V, this, SLOT( paste() ) );
  _addShortcut( CTRL+Key_U, this, SLOT( upperCase() ) );
  _addShortcut( SHIFT+CTRL+Key_U, this, SLOT( lowerCase() ) );
  _addShortcut( CTRL+Key_F, this, SLOT( findFromDialog() ) );
  _addShortcut( CTRL+Key_G, this, SLOT( findAgainForward() ) );
  _addShortcut( SHIFT+CTRL+Key_G, this, SLOT( findAgainBackward() ) );
  _addShortcut( CTRL+Key_H, this, SLOT( findSelectionForward() ) );
  _addShortcut( SHIFT+CTRL+Key_H, this, SLOT( findSelectionBackward() ) );
  _addShortcut( CTRL+Key_K, this, SLOT( removeLine() ) );
  _addShortcut( CTRL+Key_R, this, SLOT( replaceFromDialog() ) );
  _addShortcut( CTRL+Key_T, this, SLOT( replaceAgainForward() ) );
  _addShortcut( SHIFT+CTRL+Key_T, this, SLOT( replaceAgainBackward() ) );

  _addShortcut( CTRL+Key_L, this, SLOT( selectLineFromDialog() ) );
  
}

//______________________________________________________________________
QShortcut* CustomTextEdit::_addShortcut( const QKeySequence& sequence, QObject* reciever, const std::string& slot )
{
  Debug::Throw( "CustomTextEdit::_addShortcut.\n" );
  QShortcut* out = new QShortcut( sequence, this );
  if( reciever && !slot.empty() )
  { connect( out, SIGNAL( activated() ), reciever, slot.c_str() ); }
  
  shortcuts_.push_back( out );
  return out;
}

//_______________________________________________________________________________
void CustomTextEdit::_clearShortcuts( void )
{ 
  
  Debug::Throw( "CustomTextEdit::_clearShortcuts.\n" );
  for( std::vector<QShortcut*>::iterator iter = shortcuts_.begin(); iter != shortcuts_.end(); iter++ )
  { delete *iter; }
  shortcuts_.clear(); 
  
}

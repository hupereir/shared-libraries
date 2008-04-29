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
  \file CustomTextEdit.cpp
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QAbstractTextDocumentLayout>
#include <QClipboard>
#include <QMenu>
#include <QPainter>
#include <QRegExp>
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

  // paragraph highlight
  block_highlight_ = new BlockHighlight( this );
  connect( this, SIGNAL( cursorPositionChanged() ), &blockHighlight(), SLOT( highlight() ) );

  // actions
  _installActions();

  // signal to make sure selectionsynchronized is  between clones
  connect( this, SIGNAL( selectionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( this, SIGNAL( selectionChanged() ), SLOT( _updateClipboard() ) );
  connect( this, SIGNAL( cursorPositionChanged() ), SLOT( _synchronizeSelection() ) );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

  // update configuration
  _updateConfiguration();
  
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
int CustomTextEdit::blockCount( void ) const
{
  Debug::Throw( "CustomTextEdit::blockCount.\n" );

  int count = 0;
  for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next(), count++ ) {}
  return count;
}

//________________________________________________
TextPosition CustomTextEdit::textPosition( void )
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
void CustomTextEdit::selectLine( void )
{
  Debug::Throw( "CustomTextEdit::selectLine.\n" );

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


//__________________________________________________________________
void CustomTextEdit::setReadOnly( bool readonly )
{
  Debug::Throw( "CustomTextEdit::setReadOnly.\n" );
  QTextEdit::setReadOnly( readonly );
  _updateReadOnlyActions( readonly );
  if( readonly ) document()->setModified( false );
}

//___________________________________________________________________________
void CustomTextEdit::setBackground( QTextBlock block, const QColor& color )
{

  Debug::Throw( "CustomTextEdit::setBackground.\n" );

  // try retrieve data or create
  TextBlockData *data( dynamic_cast<TextBlockData*>( block.userData() ) );
  if( !data ) block.setUserData( data = new TextBlockData() );

  // try assign color
  if( data->setBackground( color ) )
  {

    // retrieve block rect, translate and redraw
    QRectF block_rect( document()->documentLayout()->blockBoundingRect( block ) );
    block_rect.setWidth( viewport()->width() );
    viewport()->update( toViewport( block_rect.toRect() ) );

  }

  return;

}

//___________________________________________________________________________
void CustomTextEdit::clearBackground( QTextBlock block )
{

  Debug::Throw( "CustomTextEdit::clearBackground.\n" );
  TextBlockData *data( dynamic_cast<TextBlockData*>( block.userData() ) );
  if( data && data->hasFlag( TextBlock::HAS_BACKGROUND ) && data->setBackground( QColor() ) )
  {
    // retrieve block rect and redraw
    QRectF block_rect( document()->documentLayout()->blockBoundingRect( block ) );
    block_rect.setWidth( viewport()->width() );
    viewport()->update( toViewport( block_rect.toRect() ) );

  }

  return;
}

//___________________________________________________________________________
void CustomTextEdit::clearAllBackgrounds( void )
{
  Debug::Throw( "CustomTextEdit::clearAllBackgrounds.\n" );
  for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next() )
  { clearBackground( block ); }
}

//________________________________________________
void CustomTextEdit::cut( void )
{

  Debug::Throw( "CustomTextEdit::cut.\n" );

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
void CustomTextEdit::copy( void )
{
  Debug::Throw( "CustomTextEdit::copy.\n" );
  if( _boxSelection().state() == BoxSelection::FINISHED ) _boxSelection().toClipboard( QClipboard::Clipboard );
  else QTextEdit::copy();
}

//________________________________________________
void CustomTextEdit::paste( void )
{

  Debug::Throw( "CustomTextEdit::paste.\n" );

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
void CustomTextEdit::upperCase( void )
{
  Debug::Throw( "CustomTextEdit::upperCase.\n" );

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
void CustomTextEdit::lowerCase( void )
{

  Debug::Throw( "CustomTextEdit::lowerCase.\n" );

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

//_____________________________________________________________________
void CustomTextEdit::findFromDialog( void )
{
  Debug::Throw( "CustomTextEdit::findFromDialog.\n" );

  // create
  if( !find_dialog_ ) _createFindDialog();

  // enable/disable regexp
  _findDialog().enableRegExp( true );

  // raise dialog
  QtUtil::centerOnParent( &_findDialog() );
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
  // update find text
  QString text;
  if( !( text = qApp->clipboard()->text( QClipboard::Selection) ).isEmpty() ) _findDialog().setText( text );
  else if( textCursor().hasSelection() ) _findDialog().setText( textCursor().selectedText() );
  else if( !( text = _lastSelection().text() ).isEmpty() ) _findDialog().setText( text );

  // changes focus
  _findDialog().activateWindow();
  _findDialog().editor().setFocus();

  return;
}


//_____________________________________________________________________
void CustomTextEdit::replaceFromDialog( void )
{
  Debug::Throw( "CustomTextEdit::replaceFromDialog.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

    // create
  if( !replace_dialog_ ) _createReplaceDialog();

  // raise dialog
  //QtUtil::centerOnPointer( &_replaceDialog() );
  QtUtil::centerOnParent( &_replaceDialog() );
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
  else if( !( text = _lastSelection().text() ).isEmpty() ) _replaceDialog().setText( text );

  // update replace text
  if( !_lastSelection().replaceText().isEmpty() ) _replaceDialog().setReplaceText( _lastSelection().replaceText() );

  // changes focus
  _replaceDialog().activateWindow();
  _replaceDialog().editor().setFocus();

  return;
}

//______________________________________________________________________
void CustomTextEdit::replace( TextSelection selection )
{

  Debug::Throw( "CustomTextEdit::replace.\n" );

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  // see if current selection match
  // perform replacement if yes
  QTextCursor cursor( textCursor() );
  if(
    cursor.hasSelection() &&
    cursor.selectedText().size() == selection.text().size() &&
    cursor.selectedText().contains(
      selection.text(),
      selection.flag( TextSelection::CASE_SENSITIVE ) ? CaseSensitive : CaseInsensitive ) )
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

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return 0;

  unsigned int counts(0);

  if( _boxSelection().state() == BoxSelection::FINISHED )
  {

    Debug::Throw( "CustomTextEdit::replaceInSelection - box selection.\n" );
    BoxSelection::CursorList cursors( _boxSelection().cursorList() );
    for( BoxSelection::CursorList::iterator iter = cursors.begin(); iter != cursors.end(); iter++ )
    { counts += _replaceInRange( selection, *iter, MOVE ); }

    _boxSelection().clear();

  } else {
    Debug::Throw( "CustomTextEdit::replaceInSelection - normal selection.\n" );
    QTextCursor cursor( textCursor() );
    counts = _replaceInRange( selection, cursor, EXPAND );
  }

  Debug::Throw( "CustomTextEdit::replaceInSelection - done.\n" );
  if( show_dialog ) showReplacements( counts );
  return counts;

}

//______________________________________________________________________
unsigned int CustomTextEdit::replaceInWindow( TextSelection selection, const bool& show_dialog )
{

  Debug::Throw( "CustomTextEdit::replaceInWindow.\n" );

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
void CustomTextEdit::selectLineFromDialog( void )
{

  Debug::Throw( "CustomTextEdit::selectLineFromDialog.\n" );
  if( !select_line_dialog_ )
  {
    select_line_dialog_ = new SelectLineDialog( this );
    connect( select_line_dialog_, SIGNAL( lineSelected( int ) ), SLOT( selectLine( int ) ) );
  }

  select_line_dialog_->editor().clear();
  QtUtil::centerOnParent( select_line_dialog_ );
  select_line_dialog_->show();
  select_line_dialog_->activateWindow();
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

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return;

  QTextCursor cursor( textCursor() );
  cursor.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );
  cursor.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
  remove_line_buffer_.append( cursor.selectedText() );

  setUpdatesEnabled( false );
  setTextCursor( cursor );
  cut();
  setUpdatesEnabled( true );

}

//________________________________________________
void CustomTextEdit::enterEvent( QEvent* event )
{

  #if QT_VERSION < 0x040200
  Debug::Throw( "CustomTextEdit::enterEvent.\n" );
  _updateClipboardActions( QClipboard::Clipboard );
  _updateClipboardActions( QClipboard::Selection );
  #endif
  
  QTextEdit::enterEvent( event );
  
}
  
//________________________________________________
void CustomTextEdit::mousePressEvent( QMouseEvent* event )
{

  Debug::Throw( "CustomTextEdit::mousePressEvent.\n" );

  if( event->button() == MidButton ) 
  { Debug::Throw( "CustomTextEdit::mousePressEvent - middle mouse button.\n" ); }
  
  // check button
  if( event->button() == LeftButton )
  {

    // increment multiple clicks
    QTextCursor cursor( textCursor() );
    switch( click_counter_.increment( cursorForPosition( event->pos() ).position() ) )
    {

      case 1:

      // if single click in existing box selection, store drag position
      if(
        event->modifiers() == NoModifier &&
        _boxSelection().state() == BoxSelection::FINISHED &&
        _boxSelection().rect().contains( fromViewport( event->pos() ) )
        )
      {
        // store position for drag
        drag_start_ = event->pos();
        return QTextEdit::mousePressEvent( event );
      }

      // if single click outside of existing box selection, clear the selection
      if( event->button() == LeftButton && _boxSelection().state() == BoxSelection::FINISHED )
      {

        _boxSelection().clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );

      }

      // if single click and Control key pressed, start a new box selection
      if( event->modifiers() == ControlModifier  )
      {

        // try re-enable box selection in case font has changed
        if( !_boxSelection().isEnabled() ) _boxSelection().updateConfiguration();
        if( _boxSelection().isEnabled() )
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
  if(  event->button() == MidButton )
  { setTextCursor( cursorForPosition( event->pos() ) ); }

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

  // see if there is a box selection in progress
  if( event->buttons() == LeftButton && _boxSelection().isEnabled() && _boxSelection().state() == BoxSelection::STARTED )
  {

    _boxSelection().update( event->pos() );
    _synchronizeBoxSelection();
    emit copyAvailable( true );
    return;

  }

  // start a new box selection if requested
  if( event->buttons() == LeftButton && _boxSelection().isEnabled() && event->modifiers() == ControlModifier && viewport()->rect().contains( event->pos() ) )
  {

    _boxSelection().start( event->pos() );
    _synchronizeBoxSelection();
    emit copyAvailable( true );
    return;
  }

  // see if dragging existing box selection
  if( event->buttons() == LeftButton && _boxSelection().state() == BoxSelection::FINISHED && (event->pos() - drag_start_ ).manhattanLength() > QApplication::startDragDistance() )
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
void CustomTextEdit::mouseReleaseEvent( QMouseEvent* event )
{

  Debug::Throw( "CustomTextEdit::mouseReleaseEvent.\n" );

  if( event->button() == MidButton ) 
  { Debug::Throw( "CustomTextEdit::mouseReleaseEvent - middle mouse button.\n" ); }

  // no need to check for enability because there is no way for the box to start if disabled
  if( event->button() == LeftButton && _boxSelection().state() == BoxSelection::STARTED )
  {

    _boxSelection().finish( event->pos() );
    _synchronizeBoxSelection();
    return QTextEdit::mouseReleaseEvent( event );

  }

  if( event->button() == LeftButton && _boxSelection().state() == BoxSelection::FINISHED )
  {

    _boxSelection().clear();
    _synchronizeBoxSelection();
    emit copyAvailable( false );
    return QTextEdit::mouseReleaseEvent( event );

  }

  if( event->button() == LeftButton && click_counter_.counts() > 1 )
  {
    // when multiple-click is in progress
    // do nothing because it can reset the selection
    event->ignore();
    return;
  }

  // do nothing with MidButton when box selection is active
  if( event->button() == MidButton  && _boxSelection().state() == BoxSelection::FINISHED ) return;

  // process event
  QTextEdit::mouseReleaseEvent( event );

}

//________________________________________________
void CustomTextEdit::dropEvent( QDropEvent* event )
{

  Debug::Throw( "CustomTextEdit::dropEvent.\n" );

  // static empty mimeData used to pass to base class
  // so that drop events are finished properly even when actually doing nothing
  static QMimeData* empty_data( new QMimeData() );
  QDropEvent empty_event( event->pos(), event->possibleActions(), empty_data, NoButton, NoModifier );

  // if mimeData is block selection, block selection is enabled here
  // and there is no active selection (standard or box), insert new box selection
  // at cursor position
  if(
    event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
    _boxSelection().isEnabled() &&
    _boxSelection().state() == BoxSelection::EMPTY &&
    !textCursor().hasSelection() )
  {

    Debug::Throw( "CustomTextEdit::dropEvent - dropping box selection.\n" );

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
    Debug::Throw( "CustomTextEdit::dropEvent - [box] moving current box selection.\n" );

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
      Debug::Throw( "CustomTextEdit::dropEvent - [box] doing nothing.\n" );
      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    } else {

      // insert mine data in current box selection
      Debug::Throw( "CustomTextEdit::dropEvent - [box] inserting selection.\n" );
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
      Debug::Throw( "CustomTextEdit::dropEvent - inserting selection.\n" );
      cursor.insertText( event->mimeData()->text() );
      event->acceptProposedAction();
      QTextEdit::dropEvent( &empty_event );
      return;

    }

    if( event->source() == this )
    {

      // drag action is from this widget
      // insert selection at current location and remove old selection

      Debug::Throw( "CustomTextEdit::dropEvent - moving selection.\n" );
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
void CustomTextEdit::keyPressEvent( QKeyEvent* event )
{

  // clear line buffer.
  remove_line_buffer_.clear();

  /*
  need to grap CTRL+X, C and V event to forward them to the
  daughter implementation of cut, copy and paste, otherwise
  they are passed to the base class, with no way to overrid
  */
  if( event->modifiers() == ControlModifier )
  {
    if( event->key() == Key_X )
    {
      cut();
      event->ignore();
      return;
    }

    if( event->key() == Key_C )
    {
      copy();
      event->ignore();
      return;
    }

    if( event->key() == Key_V )
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
      (event->key() >= Key_Shift &&  event->key() <= Key_ScrollLock) ||
      (event->key() >= Key_F1 &&  event->key() <= Key_F25) ||
      (event->key() >= Key_Super_L && event->key() <= Key_Direction_R ) ||
      (event->modifiers() != NoModifier && event->modifiers() != ShiftModifier ) )
    { return QTextEdit::keyPressEvent( event ); }

    // if cursor move clear selection
    if( event->key() >= Key_Home && event->key() <= Key_Down )
    {
      _boxSelection().clear();
      return QTextEdit::keyPressEvent( event );
    }

    // if delete or backspace remove selection
    if( event->key() == Key_Backspace || event->key() == Key_Delete )
    {
      _boxSelection().removeSelectedText();
      _boxSelection().clear();
      return;
    }

    // any other key should replace the selection
    if( event->key() == Key_Tab )
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
  QMenu menu( this );
  _installContextMenuActions( menu );
  menu.exec( event->globalPos() );

}

//______________________________________________________________
void CustomTextEdit::paintEvent( QPaintEvent* event )
{

  // handle block background
  QRect rect = event->rect();
  QTextBlock first( cursorForPosition( rect.topLeft() ).block() );
  QTextBlock last( cursorForPosition( rect.bottomRight() ).block() );

  // translate rect from widget to viewport coordinates
  rect.translate( scrollbarPosition() );

  // create painter and translate from widget to viewport coordinates
  QPainter painter( viewport() );
  painter.translate( -scrollbarPosition() );
  painter.setPen( NoPen );
  
  // loop over found blocks
  for( QTextBlock block( first ); block != last.next() && block.isValid(); block = block.next() )
  {

    // retrieve block data and check background
    TextBlockData *data( dynamic_cast<TextBlockData*>( block.userData() ) );
    if( !(data && data->hasFlag( TextBlock::HAS_BACKGROUND|TextBlock::CURRENT_BLOCK ) ) ) continue;

    // retrieve block rect
    QRectF block_rect( document()->documentLayout()->blockBoundingRect( block ) );
    block_rect.setWidth( viewport()->width() + scrollbarPosition().x() );

    QColor color;
    if( data->hasFlag( TextBlock::CURRENT_BLOCK ) && blockHighlightAction().isEnabled() && blockHighlightAction().isChecked() )
    { color = highlight_color_; }

    if( data->hasFlag( TextBlock::HAS_BACKGROUND ) )
    { color = QtUtil::mergeColors( color, data->background() ); }

    if( color.isValid() )
    {
      painter.setBrush( color );
      painter.drawRect( block_rect&rect );
    }

  }

  if(
    _boxSelection().state() == BoxSelection::STARTED ||
    _boxSelection().state() == BoxSelection::FINISHED
    )
  {

    painter.setBrush( _boxSelection().color() );
    painter.drawRect( _boxSelection().rect()&rect );

  }

  QTextEdit::paintEvent( event );

  return;
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
  select_all_action_->setShortcutContext( WidgetShortcut );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );

  addAction( upper_case_action_ = new QAction( "&Upper case", this ) );
  upper_case_action_->setShortcut( CTRL+Key_U );
  upper_case_action_->setShortcutContext( WidgetShortcut );
  connect( upper_case_action_, SIGNAL( triggered() ), SLOT( upperCase() ) );

  addAction( lower_case_action_ = new QAction( "&Lower case", this ) );
  lower_case_action_->setShortcut( SHIFT+CTRL+Key_U );
  lower_case_action_->setShortcutContext( WidgetShortcut );
  connect( lower_case_action_, SIGNAL( triggered() ), SLOT( lowerCase() ) );

  addAction( find_action_ = new QAction( IconEngine::get( ICONS::FIND, path_list ), "&Find", this ) );
  find_action_->setShortcut( CTRL+Key_F );
  find_action_->setShortcutContext( WidgetShortcut );
  connect( find_action_, SIGNAL( triggered() ), SLOT( findFromDialog() ) );

  addAction( find_again_action_ = new QAction( "F&ind again", this ) );
  find_again_action_->setShortcut( CTRL+Key_G );
  find_again_action_->setShortcutContext( WidgetShortcut );
  connect( find_again_action_, SIGNAL( triggered() ), SLOT( findAgainForward() ) );

  addAction( find_again_backward_action_ = new QAction( this ) );
  find_again_backward_action_->setShortcut( SHIFT+CTRL+Key_G );
  find_again_backward_action_->setShortcutContext( WidgetShortcut );
  connect( find_again_backward_action_, SIGNAL( triggered() ), SLOT( findAgainBackward() ) );

  addAction( find_selection_action_ = new QAction( "Find &selection", this ) );
  find_selection_action_->setShortcut( CTRL+Key_H );
  find_selection_action_->setShortcutContext( WidgetShortcut );
  connect( find_selection_action_, SIGNAL( triggered() ), SLOT( findSelectionForward() ) );

  addAction( find_selection_backward_action_ = new QAction( this ) );
  find_selection_backward_action_->setShortcut( SHIFT+CTRL+Key_H );
  find_selection_backward_action_->setShortcutContext( WidgetShortcut );
  connect( find_selection_backward_action_, SIGNAL( triggered() ), SLOT( findSelectionBackward() ) );

  addAction( replace_action_ = new QAction( IconEngine::get( ICONS::FIND, path_list ), "Replace", this ) );
  replace_action_->setShortcut( CTRL+Key_R );
  replace_action_->setShortcutContext( WidgetShortcut );
  connect( replace_action_, SIGNAL( triggered() ), SLOT( replaceFromDialog() ) );

  addAction( replace_again_action_ = new QAction( "Replace again", this ) );
  replace_again_action_->setShortcut( CTRL+Key_T );
  replace_again_action_->setShortcutContext( WidgetShortcut );
  connect( replace_again_action_, SIGNAL( triggered() ), SLOT( replaceAgainForward() ) );

  addAction( replace_again_backward_action_ = new QAction( this ) );
  replace_again_backward_action_->setShortcut( SHIFT+CTRL+Key_T );
  replace_again_backward_action_->setShortcutContext( WidgetShortcut );
  connect( replace_again_backward_action_, SIGNAL( triggered() ), SLOT( replaceAgainBackward() ) );

  addAction( goto_line_action_ = new QAction( "&Goto line number", this ) );
  goto_line_action_->setShortcut( CTRL+Key_L );
  goto_line_action_->setShortcutContext( WidgetShortcut );
  connect( goto_line_action_, SIGNAL( triggered() ), SLOT( selectLineFromDialog() ) );

  // remove line action
  QAction* remove_line_action( new QAction( "&Remove current line", this ) );
  addAction( remove_line_action );
  remove_line_action->setShortcut( CTRL+Key_K );
  remove_line_action->setShortcutContext( WidgetShortcut );
  connect( remove_line_action, SIGNAL( triggered() ), SLOT( removeLine() ) );

  // current block highlight
  addAction( block_highlight_action_ = new QAction( "&Highlight current paragraph", this ) );
  block_highlight_action_->setCheckable( true );
  block_highlight_action_->setChecked( blockHighlight().isEnabled() );
  connect( block_highlight_action_, SIGNAL( toggled( bool ) ), SLOT( _toggleBlockHighlight( bool ) ) );

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

  #if QT_VERSION >= 0x040200
  // update actions that depend on the content of the clipboard
  // this is available only starting from Qt 4.2
  connect( qApp->clipboard(), SIGNAL( changed( QClipboard::Mode ) ), SLOT( _updateClipboardActions( QClipboard::Mode ) ) );
  #endif

}

//______________________________________________________________________________
void CustomTextEdit::_installContextMenuActions( QMenu& menu )
{

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
  return;
}

//______________________________________________________________________
TextSelection CustomTextEdit::_selection( void ) const
{
  Debug::Throw( "CustomTextEdit::_selection.\n" );

  // copy last selection
  TextSelection out( "" );

  // try set from current selection
  QString text;
  if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) out.setText( text );
  else if( textCursor().hasSelection() ) out.setText( textCursor().selectedText() );

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
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? CaseSensitive:CaseInsensitive );

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
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? CaseSensitive:CaseInsensitive );

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
unsigned int CustomTextEdit::_replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode )
{

  Debug::Throw()
    << "CustomTextEdit::_replaceInRange -"
    << " anchor: " << cursor.anchor()
    << " position: " << cursor.position()
    << " selection: " << qPrintable( selection.text() )
    << " replacement: " << qPrintable( selection.replaceText() )
    << endl;

  // need to check for editability because apparently even if calling action is disabled,
  // the shortcut still can be called
  if( isReadOnly() ) return 0;

  // check selection
  if( selection.text().isEmpty() ) return 0;
  _setLastSelection( selection );

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

    Debug::Throw( "CustomTextEdit::_replaceInRange - regexp.\n" );

    // construct regexp and check
    QRegExp regexp( selection.text() );
    if( !regexp.isValid() )
    {
      QtUtil::infoDialog( this, "invalid regular expression. Find canceled" );
      return false;
    }

    // case sensitivity
    regexp.setCaseSensitivity( selection.flag( TextSelection::CASE_SENSITIVE ) ? CaseSensitive:CaseInsensitive );

    // replace everything in selected text
    QString selected_text( cursor.selectedText() );
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

    }

    // update cursor
    if( mode == EXPAND )
    {
      cursor.setPosition( saved_anchor );
      cursor.setPosition( saved_anchor + selected_text.length(), QTextCursor::KeepAnchor );

    } else if( mode == MOVE ) cursor.setPosition( current_position );

  } else {

    Debug::Throw( "CustomTextEdit::_replaceInRange - normal replacement.\n" );

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

      Debug::Throw()
        << "CustomTextEdit::_replaceInRange -"
        << " current: " << current_position
        << " saved: " << saved_position
        << " found: " << found
        << endl;
    }

    if( mode == EXPAND )
    {
      cursor.setPosition( saved_anchor );
      cursor.setPosition( saved_position, QTextCursor::KeepAnchor );
    } else if( mode == MOVE ) cursor.setPosition( current_position );

  }

  Debug::Throw( "CustomTextEdit::_replaceInRange - done.\n" );
  return found;

}

//_____________________________________________________________
void CustomTextEdit::_synchronizeBoxSelection( void ) const
{

  if( !isSynchronized() ) return;

  // Debug::Throw( "CustomTextEdit::_synchronizeBoxSelection.\n" );
  BASE::KeySet<CustomTextEdit> displays( this );
  for( BASE::KeySet<CustomTextEdit>::iterator iter = displays.begin(); iter != displays.end(); iter++ )
  { (*iter)->_boxSelection().synchronize( _boxSelection() ); }

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
void CustomTextEdit::_updateConfiguration( void )
{

  Debug::Throw( "CustomTextEdit::_updateConfiguration.\n" );

  // wrap mode
  if( wrapFromOptions() )
  { wrapModeAction().setChecked( XmlOptions::get().get<bool>( "WRAP_TEXT" ) ); }

  // tab emulation
  _setTabSize( XmlOptions::get().get<int>("TAB_SIZE") );
  tabEmulationAction().setChecked( XmlOptions::get().get<bool>( "TAB_EMULATION" ) );

  // paragraph highlighting
  highlight_color_ = QColor( XmlOptions::get().raw( "HIGHLIGHT_COLOR" ).c_str() );
  blockHighlight().setEnabled( highlight_color_.isValid() && XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );
  blockHighlightAction().setEnabled( highlight_color_.isValid() );
  blockHighlightAction().setChecked( XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );

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
void CustomTextEdit::_synchronizeSelection( void )
{

  //Debug::Throw( "CustomTextEdit::_synchronizeSelection.\n" );
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

  #if QT_VERSION < 0x040200
  // update clipboard actions, based on the clipboard content
  // this is done only for QT versions < 4.2. For higher versions
  // this is called directly from a QClipboard signal
  _updateClipboardActions( QClipboard::Clipboard );
  _updateClipboardActions( QClipboard::Selection );
  #endif
}

//________________________________________________
void CustomTextEdit::_updateClipboardActions( QClipboard::Mode mode )
{
  Debug::Throw( "CustomTextEdit::_updateClipboardActions.\n" );

  if( mode == QClipboard::Clipboard )
  { paste_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Clipboard ).isEmpty() ); }

  if( mode == QClipboard::Selection )
  {
    Debug::Throw() << "CustomTextEdit::_updateClipboardActions - clipboard: " << qPrintable( qApp->clipboard()->text( QClipboard::Selection ) ) << endl;
    find_selection_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
    find_selection_backward_action_->setEnabled( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
  }

}

//________________________________________________
void CustomTextEdit::_updateClipboard( void )
{
  Debug::Throw( "CustomTextEdit::_updateClipboard.\n" );
  
  // copy selected text to clipboard
  if( qApp->clipboard()->supportsSelection() && textCursor().hasSelection() )
  { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }
  
}

//________________________________________________
void CustomTextEdit::_updatePasteAction( void )
{

  Debug::Throw( "CustomTextEdit::_updatePasteAction.\n" );
  bool editable( !isReadOnly() );
  bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
  paste_action_->setEnabled( editable && has_clipboard );

}

//_________________________________________________
void CustomTextEdit::_toggleBlockHighlight( bool state )
{

  blockHighlight().setEnabled( highlight_color_.isValid() && state );

  // update current paragraph
  if( blockHighlight().isEnabled() ) blockHighlight().highlight();
  else
  {
    blockHighlight().clear();
    update();
  }

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
    { if( (*iter)->isSynchronized() ) (*iter)->wrapModeAction().setChecked( state ); }
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
    { if( (*iter)->isSynchronized() ) (*iter)->tabEmulationAction().setChecked( state ); }
    setSynchronized( true );

  }

  return true;

}

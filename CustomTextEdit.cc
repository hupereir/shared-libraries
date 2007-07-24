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
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include "CustomTextEdit.h"
#include "SelectLineDialog.h"
#include "TextSeparator.h"

using namespace std;
using namespace BASE;
using namespace Qt;

//______________________________________________
CustomTextEdit::CustomTextEdit( QWidget *parent ):
  QTextEdit( parent ),
  Counter( "CustomTextEdit" ),
  click_counter_( this )
{
  Debug::Throw( "CustomTextEdit::CustomTextEdit.\n" );
  
  _installShortcuts();
  
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
  
  // need to remove RICH tags, if any
  
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

//________________________________________________
void CustomTextEdit::selectLineFromDialog( void )
{
  
  Debug::Throw( "CustomTextEdit::selectLineFromDialog.\n" );
  
  SelectLineDialog dialog( this );
  dialog.setModal( true );
  connect( &dialog, SIGNAL( lineSelected( int ) ), this, SLOT( selectLine( int ) ) );
  dialog.exec();
  
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
  
  // check button
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

  menu.exec( event->globalPos() );
  
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

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

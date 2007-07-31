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
  \file CustomLineEdit.cc
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomLineEdit.h"
#include "Debug.h"
#include "Str.h"

#include <QApplication>
#include <QClipboard>

using namespace std;
using namespace Qt;

//____________________________________________________________
CustomLineEdit::CustomLineEdit( QWidget* parent ):
  QLineEdit( parent ),
  Counter( "CustomLineEdit" ),
  modified_( false )
{    
  Debug::Throw( "CustomLineEdit::CustomLineEdit.\n" );
  
  QShortcut* shortcut;
  connect( shortcut  = new QShortcut( CTRL+Key_A, this ), SIGNAL( activated() ), SLOT( selectAll() ) );
  shortcuts_.push_back( shortcut );
  
  connect( shortcut = new QShortcut( CTRL+Key_U, this ), SIGNAL( activated() ), SLOT( upperCase() ) );
  shortcuts_.push_back( shortcut );

  connect( shortcut = new QShortcut( SHIFT+CTRL+Key_U, this ), SIGNAL( activated() ), SLOT( lowerCase() ) );
  shortcuts_.push_back( shortcut );
  
  // modification state call-back
  connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _modified( const QString& ) ) );
  
}


//_____________________________________________________________________
void CustomLineEdit::setModified( const bool& value )
{ 
  Debug::Throw( "CustomTextEdit::setModified.\n" );
  if( value != modified_ )
  {
    modified_ = value;
    if( !value ) backup_ = text();
  }  
}


//_____________________________________________________________________
void CustomLineEdit::lowerCase( void )
{
  Debug::Throw( "CustomLineEdit::lowerCase.\n" );
  
  // do nothing if object is read only
  if( isReadOnly() ) return;
  
  // do nothing if selection is not valid
  if( !hasSelectedText() ) return;
  
  // do nothing if selection is not valid
  Str selection = Str( qPrintable( selectedText() ) ).lowerCase();
  
  // remove current selection
  cut();
  insert( selection.c_str() );
}

//_____________________________________________________________________
void CustomLineEdit::upperCase( void )
{
  Debug::Throw( "CustomLineEdit::upperCase.\n" );
   
  // do nothing if object is read only
  if( isReadOnly() ) return;
  
  // do nothing if selection is not valid
  if( !hasSelectedText() ) return;
  
  // get uppercased selection
  Str selection = Str( qPrintable( selectedText() ) ).upperCase();
  
  // remove current selection
  cut();
  insert( selection.c_str() );
}

//_______________________________________________________________
void CustomLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  
  // undo/redo//cut/copy/paste/clear//select_all/uppercase/lowercase
  
  // retrieve flags
  bool editable( !isReadOnly() );
  bool has_selection( hasSelectedText() );

  // menu
  QMenu menu( this );
  
  menu.addAction( "&Undo", this, SLOT( undo() ), CTRL+Key_Z )->setEnabled( editable && isUndoAvailable() );
  menu.addAction( "&Redo", this, SLOT( redo() ), SHIFT+CTRL+Key_Z )->setEnabled( editable && isRedoAvailable() );
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

//_____________________________________________
void CustomLineEdit::keyPressEvent( QKeyEvent* event )
{
  
  // process base class function
  QLineEdit::keyPressEvent( event );

  // emit signal
  emit cursorPositionChanged( cursorPosition() );
  
}

//_____________________________________________
void CustomLineEdit::mouseReleaseEvent( QMouseEvent* event )
{
  Debug::Throw( "CustomLineEdit::mouseReleaseEvent.\n" );
  
  QLineEdit::mouseReleaseEvent( event );
  emit cursorPositionChanged( cursorPosition( ) );

}

//____________________________________________________________
void CustomLineEdit::_modified( const QString& text )
{
  Debug::Throw( "CustomLineEdit::_modified.\n" );
  bool modified( text != backup_ );
  if( modified != modified_ )
  { 
    modified_ = modified;
    emit modificationChanged( modified_ );
  }
  
}
  
//____________________________________________________________
string CustomLineEdit::_getSelection( void )
{
  
  Debug::Throw( "CustomLineEdit::_getSelection.\n" );
  
  // check if current object have selected text; return it if yes.
  string out( "" );
  
  if( hasSelectedText() ) out = qPrintable( selectedText() );
  else if (QApplication::clipboard()->supportsSelection()) 
  { out = string( qPrintable( QApplication::clipboard()->text( QClipboard::Selection ) ) ); }
 
  return out;
}

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

#include <QApplication>
#include <QClipboard>

#include "BaseIcons.h"
#include "CustomLineEdit.h"
#include "CustomPixmap.h"
#include "Debug.h"
#include "IconEngine.h"
#include "Str.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;

//____________________________________________________________
CustomLineEdit::CustomLineEdit( QWidget* parent ):
  QLineEdit( parent ),
  Counter( "CustomLineEdit" ),
  modified_( false )
{    
  Debug::Throw( "CustomLineEdit::CustomLineEdit.\n" );
    
  // actions
  _installActions();
  
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
    emit modificationChanged( value );
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
  
  // menu
  QMenu menu( this );
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

//__________________________________________________________
void CustomLineEdit::_installActions( void )
{
  Debug::Throw( "CustomLineEdit::_installActions.\n" );

  // retrieve pixmaps path
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );

  // create actions
  addAction( undo_action_ = new QAction( IconEngine::get( CustomPixmap().find( ICONS::UNDO, path_list ) ), "&Undo", this ) );
  undo_action_->setShortcut( CTRL+Key_Z );
  undo_action_->setEnabled( isUndoAvailable() );
  connect( undo_action_, SIGNAL( triggered() ), SLOT( undo() ) );

  addAction( redo_action_ = new QAction( IconEngine::get( CustomPixmap().find( ICONS::REDO, path_list ) ), "&Redo", this ) );
  redo_action_->setShortcut( SHIFT+CTRL+Key_Z );
  redo_action_->setEnabled( isRedoAvailable() );
  connect( redo_action_, SIGNAL( triggered() ), SLOT( redo() ) );

  addAction( cut_action_ = new QAction( IconEngine::get( CustomPixmap().find( ICONS::CUT, path_list ) ), "Cu&t", this ) );
  cut_action_->setShortcut( CTRL+Key_X );
  connect( cut_action_, SIGNAL( triggered() ), SLOT( cut() ) );

  addAction( copy_action_ = new QAction( IconEngine::get( CustomPixmap().find( ICONS::COPY, path_list ) ), "&Copy", this ) );
  copy_action_->setShortcut( CTRL+Key_C );
  connect( copy_action_, SIGNAL( triggered() ), SLOT( copy() ) );

  addAction( paste_action_ = new QAction( IconEngine::get( CustomPixmap().find( ICONS::PASTE, path_list ) ), "&Paste", this ) );
  paste_action_->setShortcut( CTRL+Key_V );
  connect( paste_action_, SIGNAL( triggered() ), SLOT( paste() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ), SLOT( _updatePasteAction() ) );
  _updatePasteAction();

  addAction( clear_action_ = new QAction( "&Clear", this ) );
  connect( clear_action_, SIGNAL( triggered() ), SLOT( clear() ) );

  addAction( select_all_action_ = new QAction( "Select all", this ) );
  select_all_action_->setShortcut( CTRL+Key_V );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );
  
  addAction( upper_case_action_ = new QAction( "&Upper case", this ) );
  upper_case_action_->setShortcut( CTRL+Key_U );
  connect( upper_case_action_, SIGNAL( triggered() ), SLOT( upperCase() ) );

  addAction( lower_case_action_ = new QAction( "&Lower case", this ) );
  lower_case_action_->setShortcut( SHIFT+CTRL+Key_U );
  connect( lower_case_action_, SIGNAL( triggered() ), SLOT( lowerCase() ) );

  // update actions that depend on the presence of a selection
  connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _updateUndoRedoActions() ) );
  connect( this, SIGNAL( selectionChanged() ), SLOT( _updateSelectionActions() ) );
  _updateUndoRedoActions();
  _updateSelectionActions();

}

//________________________________________________
void CustomLineEdit::_updateUndoRedoActions( void )
{
  Debug::Throw( "CustomTextEdit::_updateUndoRedoActions.\n" );
  undo_action_->setEnabled( isUndoAvailable() );
  redo_action_->setEnabled( isRedoAvailable() );
}

//________________________________________________
void CustomLineEdit::_updateSelectionActions()
{
  
  Debug::Throw( "CustomTextEdit::_updateSelectionActions.\n" );

  bool has_selection( hasSelectedText() );
  bool editable( !isReadOnly() );
  
  cut_action_->setEnabled( has_selection && editable );
  copy_action_->setEnabled( has_selection );
  upper_case_action_->setEnabled( has_selection && editable );
  lower_case_action_->setEnabled( has_selection && editable );
  
}

//________________________________________________
void CustomLineEdit::_updatePasteAction( void )
{
  
  Debug::Throw( "CustomTextEdit::_updatePasteAction.\n" );
  bool editable( !isReadOnly() );
  bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
  paste_action_->setEnabled( editable && has_clipboard );
  
}

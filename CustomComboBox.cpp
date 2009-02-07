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
  \file CustomComboBox.cpp
  \brief Customized QCombobox to handle case sensitive auto completion
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomComboBox.h"

using namespace std;
using namespace Qt;

//___________________________________________________
CustomComboBox::CustomComboBox( QWidget* parent ):
  QComboBox( parent ),
  Counter( "CustomComboBox" ),
  editor_( 0 ),
  auto_completion_( false ),
  case_( Qt::CaseInsensitive )
{ 
  
  Debug::Throw( "CustomComboBox::CustomComboBox.\n" ); 
  
  // always disable autoComplation, since it is reimplemented
  QComboBox::setAutoCompletion( false );
  
  // size policy
  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
 
}

//____________________________________________________
void CustomComboBox::setEditable( bool value )
{
  Debug::Throw( "CustomComboBox::setEditable.\n" );
  QComboBox::setEditable( value );
  
  if( !value )
  {
    if( editor_ ) delete editor_;
    return;
  }
  
  if( !editor_ ) 
  {
    editor_ = new ComboLineEdit( this );
    editor_->setFrame( false );
    setLineEdit( editor_ );
    connect( editor_, SIGNAL( autoComplete( QString ) ), SLOT( _autoComplete( QString ) ) );
  }
  
}

//____________________________________________________
void ComboLineEdit::keyPressEvent( QKeyEvent* event )
{
  Debug::Throw( "ComboLineEdit::keyPressEvent.\n" );
  QLineEdit::keyPressEvent( event );
  if( !( event->key() == Key_Backspace || event->key() == Key_Delete ) ) emit autoComplete( text() );
}

//____________________________________________________
void CustomComboBox::_autoComplete( QString text )
{
  
  Debug::Throw() 
    << "CustomComboBox::_autoComplete -"
    << " text:" << text 
    << " case:" << (case_ == Qt::CaseSensitive ? "caseSensitive":"caseInsensitive" )
    << endl;
  
  if( !(editor_ && auto_completion_) ) return;
  if( text.isEmpty() || text.isNull() ) return;
      
  // loop over items, find matching
  for( int i=0; i< QComboBox::count(); i++ )
  {
    
    QString item_text( itemText( i ) );
    if( item_text.isNull() || item_text.isEmpty() ) continue;
            
    // see if text match item
    if( item_text.length() <= text.length() ) continue;
    if( item_text.indexOf( text, 0, case_ ) != 0 ) continue;
    Debug::Throw() << "CustomComboBox::_autoComplete - found matching text: " << item_text << endl; 
       
    // found text
    setEditText( item_text );
    
    // update selection
    editor_->setCursorPosition( item_text.length() );
    editor_->cursorBackward( true, item_text.length()-text.length() );
    
  }
  
  return;
  
}

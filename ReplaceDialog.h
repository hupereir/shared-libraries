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
  \file ReplaceDialog.h
  \brief replace_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#ifndef ReplaceDialog_h
#define ReplaceDialog_h

#include "Counter.h"
#include "CustomComboBox.h"
#include "FindDialog.h"

//! replace_text dialog for text editor widgets
class ReplaceDialog: public FindDialog
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor
  ReplaceDialog( QWidget* parent = 0, Qt::WFlags flags = 0 );
    
  //! string to replace
  void setReplaceText( const QString& text )
  { replace_editor_->setEditText( text ); }
  
  //! string to replace
  QString replaceText( void ) const
  { return replace_editor_->currentText(); }
  
  //! get location button hbox
  QBoxLayout& locationLayout( void )
  { 
    Exception::checkPointer( location_layout_, DESCRIPTION( "location_layout_ not initialized.\n" ) );
    return *location_layout_; 
  }
  
  //! retrieve text selection from dialog
  virtual TextSelection selection( const bool& no_increment ) const;

  //! synchronize searched strings and ComboBox
  virtual void synchronize( void );

  //! create children, polish widget
  virtual void polish( void );
    
  signals: 
  
  //! emmited when Replace is pressed
  void replace( TextSelection );

  //! emmited when Replace_in_window is pressed
  void replaceInWindow( TextSelection );

  //! emmited when Replace_in_selection is pressed
  void replaceInSelection( TextSelection );
  
  //! emmited when text replacement is changed
  void replaceTextChanged( QString text_replace );  
  
  protected slots:
  
  //! update combo box with current text
  void _updateReplaceComboBox( void )
  { _addReplacedString( replace_editor_->currentText() ); }
      
  //! create Selection object when replace button is pressed
  void _replace( void )
  { emit replace( selection( false ) ); }
      
  //! create Selection object when replace button is pressed
  void _replaceInWindow( void )
  { emit replaceInWindow( selection( false ) ); }
      
  //! create Selection object when replace button is pressed
  void _replaceInSelection( void )
  { emit replaceInSelection( selection( false ) ); }
    
  //! emmited when text replacement is changed
  void _replaceTextChanged( const QString& text )
  { emit replaceTextChanged( text ); }

    
  protected:
  
  //! Find editor
  virtual void _createEditor( void );
  
  //! location buttons
  virtual void _createLocationButtons( void );
    
  //! action buttons
  virtual void _createButtons( void );
  
  private:
            
  //! add string to both combo box and static set
  void _addReplacedString( const QString& text  )
  {
    if( text.isEmpty() ) return;
    if( replaced_strings_.find( text ) == replaced_strings_.end() )
    {
      replaced_strings_.insert( text );
      replace_editor_->addItem( text );
    }
  }

  //! line editor for text to replace
  CustomComboBox* replace_editor_;      
  
  //! hbox for selection replacement buttons
  QBoxLayout* location_layout_;
  
  //! replaced strings
  static std::set<QString> replaced_strings_;  
  
};
#endif

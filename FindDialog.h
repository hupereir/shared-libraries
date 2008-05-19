#ifndef FindDialog_h
#define FindDialog_h

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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file FindDialog.h
  \brief find_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QAbstractButton>
#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>

#include <vector>
#include <set>
#include <string>

#include "Counter.h"
#include "CustomComboBox.h"
#include "TextSelection.h"

//! find_text dialog for text editor widgets
class FindDialog: public QDialog, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:

  //! constructor
  FindDialog( QWidget* parent = 0, Qt::WFlags wflags = 0 );
  
  //! retrieve editor
  virtual CustomComboBox& editor( void ) const
  {
    assert( editor_ );
    return *editor_;
  }

  //! string to find
  virtual void setText( const QString& text )
  { 
    _addSearchedString( text );
    editor().setEditText( text );
    editor().lineEdit()->selectAll();
  }

  //! synchronize searched strings and ComboBox
  virtual void synchronize( void );
  
  //! create children, polish widget
  virtual void polish( void );

  //! string to find
  virtual QString text( void ) const
  { return editor().currentText(); }

  //! enable/disable RegExp
  virtual void enableRegExp( const bool& value )
  {
    if( !value ) regexp_checkbox_->setChecked( false );
    regexp_checkbox_->setEnabled( value );
  }

  //! add button to disabled button list
  virtual void addDisabledButton( QAbstractButton* button )
  { buttons_.push_back( button ); }

  //! get selection
  virtual TextSelection selection( const bool& no_increment ) const
  {
    
    TextSelection out( editor().currentText() );
    out.setFlag( TextSelection::BACKWARD, backward_checkbox_->isChecked() );
    out.setFlag( TextSelection::CASE_SENSITIVE, case_sensitive_checkbox_->isChecked() );
    out.setFlag( TextSelection::ENTIRE_WORD, entire_word_checkbox_->isChecked() );
    out.setFlag( TextSelection::REGEXP, regexp_checkbox_->isChecked() );
    out.setFlag( TextSelection::NO_INCREMENT, no_increment );
    return out;
  }
  
  signals:

  //! emmited when Find is pressed
  void find( TextSelection );

  public slots:
  
  //! set notification label to "no match found"
  virtual void noMatchFound( void );

  //! clear notification label 
  virtual void clearLabel( void );

  protected slots:
  
  //! update combo box with current text
  virtual void _updateFindComboBox( void )
  { _addSearchedString( editor().currentText() ); }
  
  //! create Selection object when find button is pressed
  virtual void _find( void )
  { emit find( selection( false ) ); }

  //! create Selection object when find button is pressed
  virtual void _findNoIncrement( void )
  { if( !regexp_checkbox_->isChecked() ) emit find( selection( true ) ); }

  //! update button state when regexp checkbox is checked
  virtual void _regExpChecked( bool );
  
  //! update button state depending on the string to find
  virtual void _updateButtons( const QString& text = QString() );

  protected:
    
  //! Find editor
  virtual void _createEditor( void );
  
  //! checkboxes
  virtual void _createCheckBoxes( void );
  
  //! location buttons
  virtual void _createLocationButtons( void );
    
  //! action buttons
  virtual void _createButtons( void );
  
  //! button layout
  QBoxLayout& _mainLayout()
  { 
    assert( main_layout_ );
    return *main_layout_;
  }
  
  //! button layout
  QBoxLayout& _buttonLayout()
  { 
    assert( button_layout_ );
    return *button_layout_;
  }
  
  //! add string to both combo box and static set
  virtual void _addSearchedString( const QString& text  )
  {
    if( text.isEmpty() ) return;
    if( searched_strings_.find( text ) == searched_strings_.end() )
    {
      searched_strings_.insert( text );
      editor().addItem( text );
    }
  }
    
  //! list of disabled buttons
  virtual std::vector<QAbstractButton*>& _disabledButtons( void )
  { return buttons_; }
  
  private:
 
  //! main layout
  QBoxLayout* main_layout_;
  
  //! button layout
  QBoxLayout* button_layout_;
  
  //! line editor for text to find
  CustomComboBox* editor_;

  //! backward search if checked
  QCheckBox* backward_checkbox_;

  //! case sensitive search if checked
  QCheckBox* case_sensitive_checkbox_;

  //! entire word check box
  QCheckBox* entire_word_checkbox_;
  
  //! regular expression search if checked
  QCheckBox* regexp_checkbox_;

  //! list of buttons to enable/disable depending of the editor text
  std::vector< QAbstractButton* > buttons_;

  //! set of previously searched strings
  static std::set< QString > searched_strings_;
  
  //! notification label
  QLabel* label_;
  
};
#endif

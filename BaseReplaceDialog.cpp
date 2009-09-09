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
  \file BaseReplaceDialog.cpp
  \brief replace_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLabel>
#include <QPushButton>
#include <QApplication>

#include "Debug.h"
#include "BaseReplaceDialog.h"

using namespace std;

//________________________________________________________________________
set<QString>& BaseReplaceDialog::_replacedStrings()
{
  static set<QString> strings;
  return strings;
}

//________________________________________________________________________
BaseReplaceDialog::BaseReplaceDialog( QWidget* parent, Qt::WFlags flags ):
    BaseFindDialog( parent, flags )
{
  Debug::Throw( "BaseReplaceDialog::BaseReplaceDialog.\n" );

  // set dialog title
  setWindowTitle( "Replace" );
  setOptionName( "REPLACE_DIALOG" );

  // create aditional widgets
  // insert text editor
  QLabel* label = new QLabel( "&Replace with:", this );
  _editorLayout().addWidget( label );

  // replacement editor
  _editorLayout().addWidget( replace_editor_ = new CustomComboBox( this ) );
  label->setBuddy( &_replaceEditor() );

  // disable callbacks on find editor
  disconnect( editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), this, SLOT( _findNoIncrement() ) );

  _replaceEditor().setEditable( true );
  _replaceEditor().setAutoCompletion( true, Qt::CaseSensitive );
  connect( _replaceEditor().lineEdit(), SIGNAL(returnPressed()), SLOT( _replace( void ) ) );
  connect( _replaceEditor().lineEdit(), SIGNAL(returnPressed()), SLOT( _updateFindComboBox( void ) ) );
  connect( _replaceEditor().lineEdit(), SIGNAL(returnPressed()), SLOT( _updateReplaceComboBox( void ) ) );
  connect( _replaceEditor().lineEdit(), SIGNAL(textChanged( const QString& )), SLOT( _replaceTextChanged( const QString& ) ) );

  // change tab order
  setTabOrder( &editor(), &_replaceEditor() );

  // location buttons
  QPushButton *button( 0 );
  QPushButton *previous( 0 );

  // insert selection button
  _locationLayout().addWidget( new QLabel( "Replace in: ", this ) );
  _locationLayout().addWidget( button = new QPushButton( "&Selection", this ) );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _replaceInSelection( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateReplaceComboBox( void ) ) );
  button->setToolTip( "Replace all occurence of the search string in selected text" );
  _addDisabledButton( button );
  button->setAutoDefault( false );

  // change tab order
  setTabOrder( &_entireWordCheckBox(), button );
  previous = button;

  // insert window button
  _locationLayout().addWidget( button = new QPushButton( "&Window", this ) );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _replaceInWindow( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateReplaceComboBox( void ) ) );
  button->setToolTip( "Replace all occurence of the search string in the entire window" );
  _addDisabledButton( button );
  button->setAutoDefault( false );
  replace_window_button_ = button;

  setTabOrder( previous, button );

  // replace buttons
  button = new QPushButton( "&Replace", this );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _replace( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), SLOT( _updateReplaceComboBox( void ) ) );
  _addDisabledButton( button );
  _buttonLayout().insertWidget( 1, button );
  button->setAutoDefault( false );

  setTabOrder( &_findButton(), button );

  // disable buttons
  _updateButtons();

  Debug::Throw( "BaseReplaceDialog::BaseReplaceDialog. Done.\n" );

}

//_____________________________________________________
BaseReplaceDialog::~BaseReplaceDialog( void )
{ Debug::Throw( "BaseReplaceDialog::~BaseReplaceDialog.\n" ); }

//_____________________________________________________
TextSelection BaseReplaceDialog::selection( const bool& no_increment ) const
{

  TextSelection out( BaseFindDialog::selection( no_increment ) );
  out.setReplaceText( _replaceEditor().currentText() );
  return out;

}

//__________________________________________________
void BaseReplaceDialog::synchronize( void )
{
  Debug::Throw( "BaseReplaceDialog::synchronize.\n" );

  // base class method
  BaseFindDialog::synchronize();

  // replace editor
  _replaceEditor().clear();
  for( set<QString>::iterator iter = _replacedStrings().begin(); iter != _replacedStrings().end(); iter++ )
  { _replaceEditor().addItem( *iter ); }

  // clear replace combobox text
  _replaceEditor().setEditText("");

  // set focus to find editor
  editor().setFocus();

}

//__________________________________________________
void BaseReplaceDialog::_replaceInWindow( void )
{ emit replaceInWindow( selection( false ) ); }

//__________________________________________________
void BaseReplaceDialog::_replaceInSelection( void )
{ emit replaceInSelection( selection( false ) ); }

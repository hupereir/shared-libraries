// $Id$

/******************************************************************************
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
  \file BaseFindDialog.cpp
  \brief find_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPushButton>

#include "BaseIcons.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "Debug.h"
#include "BaseFindDialog.h"

using namespace std;

//________________________________________________________________________
set<QString>& BaseFindDialog::_searchedStrings( void )
{
  static set<QString> strings;
  return strings;
}

//________________________________________________________________________
BaseFindDialog::BaseFindDialog( QWidget* parent, Qt::WFlags flags ):
    BaseDialog( parent, flags ),
    Counter( "BaseFindDialog" )
{
  Debug::Throw( "BaseFindDialog::BaseFindDialog.\n" );
  setOptionName( "FIND_DIALOG" );
  
  // set dialog title
  setWindowTitle( "find" );

  // create vbox layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin( 10 );
  layout()->setSpacing( 5 );

  // edition
  layout()->addItem( editor_layout_ = new QVBoxLayout() );
  _editorLayout().setMargin( 0 );
  _editorLayout().setSpacing( 5 );
  
  // add editor
  QLabel *label = new QLabel( "&Text to find:", this );
  _editorLayout().addWidget( label );  
  _editorLayout().addWidget( editor_ = new CustomComboBox( this ) );
  label->setBuddy( &editor() );
  editor().setEditable( true );
  editor().setCaseSensitive( Qt::CaseSensitive );
  editor().setAutoCompletion( true );
    
  connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT( _find( void ) ) );
  connect( editor().lineEdit(), SIGNAL(returnPressed()), SLOT( _updateFindComboBox( void ) ) );
  connect( editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), SLOT( _updateButtons( const QString& ) ) );
  connect( editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), SLOT( _findNoIncrement( void ) ) );
    
  // locations
  GridLayout* grid_layout( new GridLayout() );
  grid_layout->setSpacing( 5 );
  grid_layout->setMargin( 0 );
  grid_layout->setMaxCount( 2 );
  
  layout()->addItem( grid_layout );
  
  // insert checkboxes
  grid_layout->addWidget( backward_checkbox_ = new QCheckBox( "&Search backward", this ) );
  grid_layout->addWidget( case_sensitive_checkbox_ = new QCheckBox( "&Case sensitive", this ) );
  grid_layout->addWidget( regexp_checkbox_ = new QCheckBox( "&Regular expresion", this ) );
  grid_layout->addWidget( entire_word_checkbox_ = new QCheckBox( "&Entire word", this ) );
  connect( regexp_checkbox_, SIGNAL( toggled( bool ) ), SLOT( _regExpChecked( bool ) ) );

  // tooltips
  backward_checkbox_->setToolTip( "Perform search backward" ); 
  case_sensitive_checkbox_->setToolTip( "Case sensitive search" ); 
  regexp_checkbox_->setToolTip( "Search text using regular expression" ); 

  // notification label
  layout()->addWidget( label_ = new QLabel( this ) );
  label_->setMargin( 2 );
  
  // location layout
  layout()->addItem( location_layout_ = new QHBoxLayout() );
  _locationLayout().setMargin(0);
  _locationLayout().setSpacing(5);
  
  // buttons
  layout()->addItem( button_layout_ = new QHBoxLayout() );
  _buttonLayout().setMargin( 0 );
  _buttonLayout().setSpacing( 5 );
 
  // insert Find button
  QPushButton *button;
  _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::FIND ), "&Find", this ) );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _find( void ) ) );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _updateFindComboBox( void ) ) );
  _addDisabledButton( button );
  button->setDefault( true );
  button->setAutoDefault( false );
  find_button_ = button;
  
  // insert Cancel button
  _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Close", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setAutoDefault( false );
  
  // disable buttons
  _updateButtons();
  
}

//________________________________________________________________________
void BaseFindDialog::synchronize( void )
{

  Debug::Throw( "BaseFindDialog::synchronize.\n" );
  editor().clear();
  
  for( set<QString>::iterator iter = _searchedStrings().begin(); iter != _searchedStrings().end(); iter++ )
  { editor().addItem( *iter ); }

}

//________________________________________________________________________ 
void BaseFindDialog::noMatchFound( void )
{ label_->setText( "No match found ..." ); }

//________________________________________________________________________ 
void BaseFindDialog::clearLabel( void )
{ label_->setText( "" ); }

//________________________________________________________________________
void BaseFindDialog::_regExpChecked( bool value )
{
  Debug::Throw( "BaseFindDialog::_regExpChecked.\n" );
  case_sensitive_checkbox_->setChecked( value );
  if( value )
  {
    entire_word_checkbox_->setChecked( false );
    entire_word_checkbox_->setEnabled( false );
  } else { entire_word_checkbox_->setEnabled( true ); }

}

//________________________________________________________________________
void BaseFindDialog::_updateButtons( const QString& text )
{
  Debug::Throw( "BaseFindDialog::_updateButtons.\n" );
  
  bool enabled( !( text.isNull() || text.isEmpty() ) );

  for( vector< QAbstractButton* >::iterator iter = _disabledButtons().begin(); iter != _disabledButtons().end(); iter++ )
  { (*iter)->setEnabled( enabled ); }

  Debug::Throw( "BaseFindDialog::_updateButtons - done.\n" );
}

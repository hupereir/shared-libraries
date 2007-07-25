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
  \file FindDialog.cc
  \brief find_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPushButton>

#include "CustomLineEdit.h"
#include "Debug.h"
#include "FindDialog.h"
#include "QtUtil.h"

using namespace std;

//________________________________________________________________________
set<QString> FindDialog::searched_strings_;

//________________________________________________________________________
FindDialog::FindDialog( QWidget* parent, Qt::WFlags flags ):
    QDialog( parent, flags ),
    Counter( "FindDialog" )
{
  Debug::Throw( "FindDialog::FindDialog.\n" );

  // set dialog title
  setWindowTitle( "find" );

}

//________________________________________________________________________
void FindDialog::synchronize( void )
{

  Debug::Throw( "FindDialog::synchronize.\n" );
  editor_->clear();
  
  for( set<QString>::iterator iter = searched_strings_.begin(); iter != searched_strings_.end(); iter++ )
  { editor_->addItem( *iter ); }

}

//________________________________________________________________________
void FindDialog::polish( void )
{
  Debug::Throw("FindDialog::polish.\n" ); 

  // create vbox layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin( 5 );
  layout()->setSpacing( 2 );
  
  _createEditor();
  _createCheckBoxes();
  _createLocationButtons();
  _createButtons(); 
  
  // disable buttons
  _updateButtons();

  // activate layout
  layout()->activate();
  
}

//________________________________________________________________________ 
void FindDialog::_createEditor( void )
{
  
  Debug::Throw("FindDialog::_createEditor.\n" );
  
  // insert text editor
  QLabel *label = new QLabel( "string to find:", this );
  QtUtil::fixSize( label );
  layout()->addWidget( label );
  
  layout()->addWidget( editor_ = new QComboBox( this ) );
  editor_->setEditable( true );
  editor_->setAutoCompletion( true );
    
  connect( editor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT( _find( void ) ) );
  connect( editor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT( _updateFindComboBox( void ) ) );
  connect( editor_->lineEdit(), SIGNAL(textChanged( const QString& ) ), this, SLOT( _updateButtons( const QString& ) ) );
  connect( editor_->lineEdit(), SIGNAL(textChanged( const QString& ) ), this, SLOT( _findNoIncrement( const QString& ) ) );

}

//________________________________________________________________________ 
void FindDialog::_createCheckBoxes( void )
{
  
  Debug::Throw("FindDialog::_CreateCheckBoxes.\n" );
  
  QGridLayout* grid_layout( new QGridLayout() );
  grid_layout->setSpacing( 2 );
  grid_layout->setMargin( 0 );
  layout()->addItem( grid_layout );
  
  // insert checkboxes
  grid_layout->addWidget( backward_checkbox_ = new QCheckBox( "&search backward", this ), 0, 0 );
  grid_layout->addWidget( case_sensitive_checkbox_ = new QCheckBox( "&case sensitive", this ), 0, 1 );
  grid_layout->addWidget( regexp_checkbox_ = new QCheckBox( "&regular expresion", this ), 1, 0 );
  grid_layout->addWidget( entire_word_checkbox_ = new QCheckBox( "&entire word", this ), 1, 1 );
  connect( regexp_checkbox_, SIGNAL( toggled( bool ) ), this, SLOT( _regExpChecked( bool ) ) );

  // tooltips
  backward_checkbox_->setToolTip( "perform search backward" ); 
  case_sensitive_checkbox_->setToolTip( "case sensitive search" ); 
  regexp_checkbox_->setToolTip( "search text using regular expression" ); 

  // notification label
  layout()->addWidget( label_ = new QLabel( this ) );
  label_->setMargin( 2 );
}

//________________________________________________________________________ 
void FindDialog::_createLocationButtons( void )
{
  Debug::Throw("FindDialog::_CreateLocationButtons.\n" ); 
  return;
}


//________________________________________________________________________ 
void FindDialog::_createButtons( void )
{
  Debug::Throw("FindDialog::_CreateButtons.\n" );  

  // insert hbox layout for buttons
  button_layout_ = new QHBoxLayout();
  button_layout_->setMargin(0);
  button_layout_->setSpacing(5);
  layout()->addItem( button_layout_ );
  
  // insert Find button
  QPushButton *button = new QPushButton( "&Find", this );
  QtUtil::fixSize( button );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( _find( void ) ) );
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( _updateFindComboBox( void ) ) );
  addDisabledButton( button );
  button_layout_->addWidget( button );

  // insert Cancel button
  button = new QPushButton( "&Cancel", this );
  QtUtil::fixSize( button );
  connect( button, SIGNAL( clicked() ), this, SLOT( close() ) );
  button->setAutoDefault( false );
  button_layout_->addWidget( button );
  
}

//________________________________________________________________________
void FindDialog::_regExpChecked( bool value )
{
  Debug::Throw( "FindDialog::_regExpChecked.\n" );
  case_sensitive_checkbox_->setChecked( value );
  if( value )
  {
    entire_word_checkbox_->setChecked( false );
    entire_word_checkbox_->setEnabled( false );
  } else { entire_word_checkbox_->setEnabled( true ); }

}

//________________________________________________________________________
void FindDialog::_updateButtons( const QString& text )
{
  Debug::Throw( "FindDialog::_updateButtons.\n" );
  bool enabled( !text.isEmpty() );

  for( vector< QAbstractButton* >::iterator iter = _disabledButtons().begin(); iter != _disabledButtons().end(); iter++ )
  (*iter)->setEnabled( enabled );

}

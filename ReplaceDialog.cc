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
  \file ReplaceDialog.cc
  \brief replace_text dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLabel>
#include <QPushButton>

#include "Debug.h"
#include "ReplaceDialog.h"
#include "QtUtil.h"

using namespace std;

//________________________________________________________________________
set<QString> ReplaceDialog::replaced_strings_;

//________________________________________________________________________
ReplaceDialog::ReplaceDialog( QWidget* parent, Qt::WFlags flags ):
    FindDialog( parent, flags )
{
  Debug::Throw( "ReplaceDialog::ReplaceDialog.\n" );
  
  // set dialog title
  setWindowTitle( "replace" );

}

//_____________________________________________________
TextSelection ReplaceDialog::selection( const bool& no_increment ) const
{
  
  TextSelection out( FindDialog::selection( no_increment ) );
  out.setReplaceText( replace_editor_->currentText() );
  return out;
  
}

//__________________________________________________
void ReplaceDialog::synchronize( void )
{
  Debug::Throw( "ReplaceDialog::synchronize.\n" );
  
  // base class method
  FindDialog::synchronize();
  
  // replace editor
  replace_editor_->clear();
  for( set<QString>::iterator iter = replaced_strings_.begin(); iter != replaced_strings_.end(); iter++ )
  { replace_editor_->addItem( *iter ); }
  
  // clear replace combobox text
  replace_editor_->setEditText("");
  
  // set focus to find editor
  _editor().setFocus();
  
}


//________________________________________________________________________
void ReplaceDialog::polish( void )
{
  Debug::Throw("ReplaceDialog::polish.\n" ); 

  // create vbox layout
  FindDialog::polish();
  
  // disable callbacks on find editor
  disconnect( _editor().lineEdit(), SIGNAL(textChanged( const QString& ) ), this, SLOT( _findNoIncrement( const QString& ) ) ); 
  
  return;

}

//_____________________________________________________ 
void ReplaceDialog::_createEditor( void ) 
{
  Debug::Throw( "ReplaceDialog::_CreateEditor.\n" );
  
  FindDialog::_createEditor();
  
  // insert text editor
  QLabel* label = new QLabel( "replace with:", this );
  QtUtil::fixSize( label );
  layout()->addWidget( label );
  
  // replacement editor
  layout()->addWidget( replace_editor_ = new CustomComboBox( this ) );
  replace_editor_->setEditable( true );
  replace_editor_->setCaseSensitive( Qt::CaseSensitive );
  replace_editor_->setAutoCompletion( true );
  connect( replace_editor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT( _replace( void ) ) );
  connect( replace_editor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT( _updateFindComboBox( void ) ) );
  connect( replace_editor_->lineEdit(), SIGNAL(returnPressed()), this, SLOT( _updateReplaceComboBox( void ) ) );
  connect( replace_editor_->lineEdit(), SIGNAL(textChanged( const QString& )), this, SLOT( _replaceTextChanged( const QString& ) ) );

}
 
//_____________________________________________________________  
void ReplaceDialog::_createLocationButtons( void )  
{
  Debug::Throw( "ReplaceDialog::_createLocationButtons.\n" );
  
  FindDialog::_createLocationButtons();
   
  location_layout_ = new QHBoxLayout();
  location_layout_->setSpacing(5);
  location_layout_->setMargin(0);
  layout()->addItem( location_layout_ );
  
  QPushButton *button( 0 );
  
  // insert selection button
  location_layout_->addWidget( button = new QPushButton( "&Selection", this ) );
  QtUtil::fixSize( button );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( _replaceInSelection( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateReplaceComboBox( void ) ) );
  button->setToolTip( "replace all occurence of the search string in selected text" );
  addDisabledButton( button );

  // insert window button
  location_layout_->addWidget( button = new QPushButton( "&Window", this ) );
  QtUtil::fixSize( button );
  button->setAutoDefault( false );  
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( _replaceInWindow( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateReplaceComboBox( void ) ) );
  button->setToolTip( "replace all occurence of the search string in the entire window" );
  addDisabledButton( button );
  
}


//_____________________________________________________________  
void ReplaceDialog::_createButtons( void )  
{
  Debug::Throw( "ReplaceDialog::_createButtons.\n" );
  
  FindDialog::_createButtons();
     
  // insert Replace button
  QPushButton* button = new QPushButton( "&Replace", this );
  QtUtil::fixSize( button );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( _replace( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateFindComboBox( void ) ) );
  connect( button, SIGNAL( clicked( void )), this, SLOT( _updateReplaceComboBox( void ) ) );
  addDisabledButton( button );
  _buttonLayout().insertWidget( 1, button );

}

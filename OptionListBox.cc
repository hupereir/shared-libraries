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
   \file OptionListBox.cc
   \brief ListBox for Special options, and buttons to add/remove values
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QHBoxLayout>
#include <QLabel>
#include <sstream>

#include "BrowsedLineEdit.h"
#include "CustomLineEdit.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "OptionListBox.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________________
OptionListBox::OptionListBox( QWidget* parent, const string& name ):
    QWidget( parent ),
    OptionWidget( name ),
    browsable_( false )
{

  Debug::Throw( "OptionListBox::OptionListBox.\n" );

  QHBoxLayout *main_layout( new QHBoxLayout() );
  setLayout( main_layout );
  
  // create list
  list_ = new CustomListBox( this );
  list_->setSelectionMode( QAbstractItemView::ExtendedSelection );  
  connect( list_, SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), this, SLOT( _edit( QListWidgetItem* ) ) );
  connect( list_, SIGNAL( itemSelectionChanged() ), this, SLOT( _updateButtons() ) ); 
  main_layout->addWidget( list_ );
  
  QVBoxLayout* box_layout = new QVBoxLayout();
  main_layout->addLayout( box_layout );
  box_layout->setMargin( 5 );
  box_layout->setSpacing( 5 );

  // Add button
  QPushButton *button = new QPushButton( "&Add", this );
  connect( button, SIGNAL( clicked() ), this, SLOT( _add() ) );
  QtUtil::fixSize( button );
  button->setToolTip( "add a new value" );
  box_layout->addWidget( button );
  
  // Edit button
  edit_ = new QPushButton( "&Edit", this );
  connect( edit_, SIGNAL( clicked() ), this, SLOT( _edit() ) );
  QtUtil::fixSize( edit_ );
  edit_->setToolTip( "edit selected value" );
  box_layout->addWidget( edit_ );

  // remove button
  remove_ = new QPushButton( "&Remove", this );
  connect( remove_, SIGNAL( clicked() ), this, SLOT( _remove() ) );
  QtUtil::fixSize( remove_ );
  remove_->setToolTip( "remove selected value" );
  box_layout->addWidget( remove_ );

  // set default button
  default_ = new QPushButton( "&Default", this );
  connect( default_, SIGNAL( clicked() ), this, SLOT( _setDefault() ) );
  QtUtil::fixSize( default_ );
  default_->setToolTip( "set selected value as default\n(move it to the top of the list)" );
  box_layout->addWidget( default_ );

  // update buttons
  _updateButtons();
  
}

//_______________________________________________________
void OptionListBox::read( void )
{
  Debug::Throw( "OptionListBox::read.\n" );

  // clear list
  list_->clear();

  // retrieve all values from Options, insert in list
  list<string> values( XmlOptions::get().specialOptions<string>( optionName() ) );
  for( list<string>::iterator iter = values.begin(); iter != values.end(); iter++ )
  { ( new QListWidgetItem( list_) )->setText( iter->c_str() ); }
  
}

//_______________________________________________________
void OptionListBox::write( void ) const
{
  Debug::Throw( "OptionListBox::write.\n" );
  XmlOptions::get().clearSpecialOptions( optionName() );
  XmlOptions::get().keep( optionName() );
  bool first( true );
  for( int row = 0; row < list_->QListWidget::count(); row++ )
  {

    if( !list_->item( row ) ) continue;
    
    Option option( optionName(), qPrintable( list_->item(row)->text() ), "", first );
    XmlOptions::get().add( option );
    first = false;
  }
}

//______________________________________________________________________   
void OptionListBox::_updateButtons( void )
{
  Debug::Throw( "OptionListBox::_updateButtons.\n" );
  
  // retrieve list of items
  QList<QListWidgetItem*> items( list_->QListWidget::selectedItems() );
  
  // enable buttons depending on the size of the list
  edit_->setEnabled( items.size() == 1 );
  remove_->setEnabled( items.size() != 0 );
  default_->setEnabled( items.size() == 1 );
}   

//_______________________________________________________
void OptionListBox::_add( void )
{
  Debug::Throw( "OptionListBox::_add.\n" );

  // create dialog
  CustomDialog dialog( this );

  ostringstream what;
  what << "add value for option " << optionName();
  dialog.mainWidget().layout()->addWidget( new QLabel( what.str().c_str(), &dialog.mainWidget() ) );

  // create editor, either directly or from BrowsedLineEdit
  CustomLineEdit* line_edit( 0 );
  if( browsable_ ) 
  {
    BrowsedLineEdit* browse_edit = new BrowsedLineEdit( &dialog.mainWidget() );
    dialog.mainWidget().layout()->addWidget( browse_edit );
    line_edit = &browse_edit->editor();
  } else {
    line_edit = new CustomLineEdit( &dialog.mainWidget() );
    dialog.mainWidget().layout()->addWidget( line_edit );
  }
  
  // map dialog
  QtUtil::centerOnPointer( &dialog );
  if( dialog.exec() == QDialog::Rejected ) return;
  if( line_edit->text().isEmpty() ) return;
  
  // create new item
  ( new QListWidgetItem( list_ ) )->setText( line_edit->text() );

}

//_______________________________________________________
void OptionListBox::_edit( QListWidgetItem* )
{
  Debug::Throw( "OptionListBox::_edit.\n" );

  // retrieve selection
  QList<QListWidgetItem*> items( list_->QListWidget::selectedItems() );
  Exception::assert( items.size() == 1, DESCRIPTION( "invalid selection" ) );

  // create dialog
  CustomDialog dialog( this );

  // add explanation label
  ostringstream what;
  what << "Edit value for option " << optionName();
  dialog.mainWidget().layout()->addWidget( new QLabel( what.str().c_str(), &dialog.mainWidget() ) );

  // create editor, either directly or from BrowsedLineEdit
  CustomLineEdit* line_edit( 0 );
  if( browsable_ ) 
  {
    BrowsedLineEdit* browse_edit = new BrowsedLineEdit( &dialog.mainWidget() );
    dialog.mainWidget().layout()->addWidget( browse_edit );
    line_edit = &browse_edit->editor();
  } else {
    line_edit = new CustomLineEdit( &dialog.mainWidget() );
    dialog.mainWidget().layout()->addWidget( line_edit );
  }

  line_edit->setText( items.front()->text() );
  QtUtil::expand( line_edit, qPrintable( items.front()->text() ) );

  // map dialog
  QtUtil::centerOnPointer( &dialog );
  if( dialog.exec() == QDialog::Rejected ) return;
  
  if( line_edit->text().isEmpty() ) return;

  items.front()->setText( qPrintable( line_edit->text() ) );
  return;

}

//_______________________________________________________
void OptionListBox::_remove( void )
{
  Debug::Throw( "OptionListBox::_remove.\n" );

  // retrieve selection
  QList<QListWidgetItem*> items( list_->QListWidget::selectedItems() );
  Exception::assert( !items.empty(), DESCRIPTION( "invalid selection" ) );
  for( QList<QListWidgetItem*>::iterator iter  = items.begin(); iter != items.end(); iter++ )
  { delete *iter; }
  
  return;

}

//_______________________________________________________
void OptionListBox::_setDefault( void )
{
  Debug::Throw( "OptionListBox::_setDefault.\n" );

  // retrieve selection
  QList<QListWidgetItem*> items( list_->QListWidget::selectedItems() );
  Exception::assert( items.size() == 1, DESCRIPTION( "invalid selection" ) );

  // move Item to the top of the list
  list_->takeItem( list_->row( items.front() ) );
  list_->insertItem( 0, items.front() );
}

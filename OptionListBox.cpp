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
   \file OptionListBox.cpp
   \brief ListBox for Special options, and buttons to add/remove values
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <sstream>

#include "BrowsedLineEditor.h"
#include "ImprovedLineEditor.h"
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
    browsable_( false ),
    file_mode_( QFileDialog::AnyFile )
{

  Debug::Throw( "OptionListBox::OptionListBox.\n" );

  QHBoxLayout* layout( new QHBoxLayout() );
  layout->setSpacing(5);
  layout->setMargin(0);
  setLayout( layout );
  
  // create list
  list_ = new TreeWidget( this );
  list_->setSortingEnabled( false );
  list_->header()->hide();
  list_->setColumnCount(1);
  
  list_->setSelectionMode( QAbstractItemView::ExtendedSelection );  
  connect( list_, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), SLOT( _edit() ) );
  connect( list_, SIGNAL( itemSelectionChanged() ), SLOT( _updateButtons() ) ); 
  layout->addWidget( list_, 1 );
  
  QVBoxLayout* button_layout = new QVBoxLayout();
  button_layout->setMargin(0);
  button_layout->setSpacing( 5 );
  layout->addLayout( button_layout, 0 );

  // Add button
  QPushButton *button;
  button_layout->addWidget( button = new QPushButton( "&Add", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _add() ) );
  
  // Edit button
  button_layout->addWidget( edit_ = new QPushButton( "&Edit", this ) );
  connect( edit_, SIGNAL( clicked() ), SLOT( _edit() ) );
  edit_->setToolTip( "edit selected value" );

  // remove button
  addAction( remove_action_ = new QAction( "Remo&ve", this ) );
  connect( remove_action_, SIGNAL( triggered() ), SLOT( _remove() ) );
  remove_action_->setShortcut( Qt::Key_Delete );
  remove_action_->setToolTip( "Remove selected value" );
  
  button_layout->addWidget( remove_ = new QPushButton( "&Remove", this ) );
  connect( remove_, SIGNAL( clicked() ), SLOT( _remove() ) );
  remove_->setToolTip( "remove selected value" );

  // set default button
  button_layout->addWidget( default_ = new QPushButton( "&Default", this ) );
  connect( default_, SIGNAL( clicked() ), SLOT( _setDefault() ) );
  default_->setToolTip( "set selected value as default\n(move it to the top of the list)" );

  button_layout->addStretch(1);
  
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
  { ( new QTreeWidgetItem( list_) )->setText( 0, iter->c_str() ); }
  
}

//_______________________________________________________
void OptionListBox::write( void ) const
{
  Debug::Throw( "OptionListBox::write.\n" );
  XmlOptions::get().clearSpecialOptions( optionName() );
  XmlOptions::get().keep( optionName() );
  bool first( true );
  QList<QTreeWidgetItem*> items( list_->children() );
  for( QList<QTreeWidgetItem*>::iterator iter  = items.begin(); iter != items.end(); iter++ )
  {
    
    Option option( optionName(), qPrintable( (*iter)->text(0) ), "", first );
    XmlOptions::get().add( option );
    first = false;
  }
}

//______________________________________________________________________   
void OptionListBox::_updateButtons( void )
{
  Debug::Throw( "OptionListBox::_updateButtons.\n" );
  
  // retrieve list of items
  QList<QTreeWidgetItem*> items( list_->QTreeWidget::selectedItems() );
  
  // enable buttons depending on the size of the list
  edit_->setEnabled( items.size() == 1 );
  remove_action_->setEnabled( items.size() != 0 );
  remove_->setEnabled( items.size() != 0 );
  default_->setEnabled( items.size() == 1 );
  
}   

//_______________________________________________________
void OptionListBox::_add( void )
{
  Debug::Throw( "OptionListBox::_add.\n" );

  // create dialog
  CustomDialog dialog( this );

  // create editor, either directly or from BrowsedLineEditor
  BrowsedLineEditor::Editor* line_edit( 0 );
  if( browsable_ ) 
  {
    BrowsedLineEditor* browse_edit = new BrowsedLineEditor( &dialog );
    dialog.mainLayout().addWidget( browse_edit );
    browse_edit->setFileMode( file_mode_ );
    line_edit = &browse_edit->editor();
  } else {
    line_edit = new BrowsedLineEditor::Editor( &dialog );
    dialog.mainLayout().addWidget( line_edit );
  }
  
  // map dialog
  dialog.adjustSize();
  QtUtil::centerOnParent( &dialog );
  if( dialog.exec() == QDialog::Rejected ) return;
  if( line_edit->text().isEmpty() ) return;
  
  // create new item
  ( new QTreeWidgetItem( list_ ) )->setText( 0, line_edit->text() );

}

//_______________________________________________________
void OptionListBox::_edit( void )
{
  Debug::Throw( "OptionListBox::_edit.\n" );

  // retrieve selection
  QList<QTreeWidgetItem*> items( list_->QTreeWidget::selectedItems() );
  assert( items.size() == 1 );

  // create dialog
  CustomDialog dialog( this );

  // create editor, either directly or from BrowsedLineEditor
  BrowsedLineEditor::Editor* line_edit( 0 );
  if( browsable_ ) 
  {
    BrowsedLineEditor* browse_edit = new BrowsedLineEditor( &dialog );
    dialog.mainLayout().addWidget( browse_edit );
    browse_edit->setFileMode( file_mode_ );
    line_edit = &browse_edit->editor();
  } else {
    line_edit = new BrowsedLineEditor::Editor( &dialog );
    dialog.mainLayout().addWidget( line_edit );
  }

  line_edit->setText( items.front()->text(0) );
  QtUtil::expand( line_edit, qPrintable( items.front()->text(0) ) );

  // map dialog
  dialog.adjustSize();
  QtUtil::centerOnParent( &dialog );
  if( dialog.exec() == QDialog::Rejected ) return;
  
  if( line_edit->text().isEmpty() ) return;

  items.front()->setText( 0, qPrintable( line_edit->text() ) );
  return;

}

//_______________________________________________________
void OptionListBox::_remove( void )
{
  Debug::Throw( "OptionListBox::_remove.\n" );

  // retrieve selection
  QList<QTreeWidgetItem*> items( list_->QTreeWidget::selectedItems() );
  assert( !items.empty() );
  ostringstream what;
  what << "remove selected value" << (items.size() > 1 ? "s":"") << " ?";
  if( !QtUtil::questionDialog( this, what.str().c_str() ) ) return;
  for( QList<QTreeWidgetItem*>::iterator iter  = items.begin(); iter != items.end(); iter++ )
  { delete *iter; }
  
  return;

}

//_______________________________________________________
void OptionListBox::_setDefault( void )
{
  Debug::Throw( "OptionListBox::_setDefault.\n" );

  // retrieve selection
  QList<QTreeWidgetItem*> items( list_->QTreeWidget::selectedItems() );
  assert( items.size() == 1 );

  // move Item to the top of the list
  list_->takeTopLevelItem( list_->indexOfTopLevelItem( items.front() ) );
  list_->insertTopLevelItem( 0, items.front() );
}

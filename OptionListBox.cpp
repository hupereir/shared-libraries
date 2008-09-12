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

#include <algorithm>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <sstream>

#include "Debug.h"
#include "IconSize.h"
#include "LineEditor.h"
#include "OptionListBox.h"
#include "QtUtil.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
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
 
  // set model editable
  model_.setReadOnly( false );
  
  // create list
  list_ = new TreeView( this );
  _list().setSortingEnabled( false );
  _list().header()->hide();
  _list().setSelectionMode( QAbstractItemView::ExtendedSelection );  
  _list().setModel( &model_ );
  _list().setRootIsDecorated( false );
  _list().setMask( 1<<OptionModel::DEFAULT|1<<OptionModel::VALUE );
  _list().setItemDelegate( new TextEditionDelegate( this ) );
  _list().setIconSize( IconSize( IconSize::SMALL ) );
  layout->addWidget( &_list(), 1 );
  
  // set connections
  connect( _list().selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ), SLOT( _updateButtons() ) );
  connect( _list().selectionModel(), SIGNAL( currentChanged( const QModelIndex& , const QModelIndex& ) ), SLOT( _updateButtons() ) );
  
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

  // retrieve all values from Options, insert in list
  Options::List values( XmlOptions::get().specialOptions( optionName() ) );  
  
  // check if one option is default, set first otherwise
  if( !values.empty() && find_if( values.begin(), values.end(), Option::HasFlagFTor( Option::DEFAULT ) ) == values.end() )
  { values.front().setFlag( Option::DEFAULT ); }
  
  // add to model.
  OptionModel::Set options;
  for( Options::List::const_iterator iter = values.begin(); iter != values.end(); iter++ )
  { options.insert( OptionPair( optionName(), *iter ) ); }
  
  model_.set( options );
  _list().resizeColumns();
  
}

//_______________________________________________________
void OptionListBox::write( void ) const
{
  Debug::Throw( "OptionListBox::write.\n" );
  
  XmlOptions::get().clearSpecialOptions( optionName() );
  XmlOptions::get().keep( optionName() );
  
  OptionModel::List values( model_.children() );
  for( OptionModel::List::const_iterator iter  = values.begin(); iter != values.end(); iter++ )
  { XmlOptions::get().add( iter->first, iter->second ); }
  
}

//______________________________________________________________________   
void OptionListBox::_updateButtons( void )
{
  Debug::Throw( "OptionListBox::_updateButtons.\n" );


  QModelIndex current( _list().selectionModel()->currentIndex() );
  edit_->setEnabled( current.isValid() && model_.get( current ).second.hasFlag( Option::RECORDABLE ) );
  default_->setEnabled( current.isValid() );

  OptionModel::List selection( model_.get( _list().selectionModel()->selectedRows() ) );
  bool remove_enabled( false );
  for( OptionModel::List::const_iterator iter = selection.begin(); iter != selection.end(); iter++ )
  { 
    if( iter->second.hasFlag( Option::RECORDABLE ) ) 
    { 
      remove_enabled = true;
      break;
    }
  }
  
  remove_action_->setEnabled( remove_enabled );
  remove_->setEnabled( remove_enabled );
  
}   

//_______________________________________________________
void OptionListBox::_add( void )
{
  Debug::Throw( "OptionListBox::_add.\n" );
  
  // map dialog
  EditDialog dialog( this, browsable_, file_mode_ );
  if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
  if( dialog.editor().text().isEmpty() ) return;
  
  // create new item
  model_.add( Options::Pair( optionName(), qPrintable( dialog.editor().text() ) ) );

}

//_______________________________________________________
void OptionListBox::_edit( void )
{
  Debug::Throw( "OptionListBox::_edit.\n" );

  // retrieve selection
  QModelIndex current( _list().selectionModel()->currentIndex() );
  assert( current.isValid() );

  Options::Pair option( model_.get( current ) );
  assert( option.second.hasFlag( Option::RECORDABLE ) );
  
  // create dialog
  EditDialog dialog( this, browsable_, file_mode_ );
  dialog.editor().setText( option.second.raw().c_str() );

  // map dialog
  if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
  if( dialog.editor().text().isEmpty() ) return;
  
  // first remove old option
  model_.remove( option );

  // modify value 
  option.second.setRaw( qPrintable( dialog.editor().text() ) );
  
  // re-add to model
  model_.add( option );
  
  return;

}

//_______________________________________________________
void OptionListBox::_remove( void )
{
  Debug::Throw( "OptionListBox::_remove.\n" );

  // retrieve selected items; retrieve only recordable options
  OptionModel::List selection( model_.get( _list().selectionModel()->selectedRows() ) );
  OptionModel::List removed;
  for( OptionModel::List::const_iterator iter = selection.begin(); iter != selection.end(); iter++ )
  { if( iter->second.hasFlag( Option::RECORDABLE ) ) removed.push_back( *iter ); }
  
  // remove
  model_.remove( OptionModel::Set( removed.begin(), removed.end() ) );
  
  return;

}

//_______________________________________________________
void OptionListBox::_setDefault( void )
{
  Debug::Throw( "OptionListBox::_setDefault.\n" );

  // retrieve selection
  QModelIndex current( _list().selectionModel()->currentIndex() );
  assert( current.isValid() );

  OptionModel::List options( model_.children() );
  for( OptionModel::List::iterator iter = options.begin(); iter != options.end(); iter++ )
  { iter->second.setFlag( Option::DEFAULT, false ); }
  
  model_.set( OptionModel::Set( options.begin(), options.end() ) );
  
  Options::Pair current_option( model_.get( current ) );
  current_option.second.setFlag( Option::DEFAULT );
  model_.add( current_option );

  _list().resizeColumns();

}

//_______________________________________________________
OptionListBox::EditDialog::EditDialog( QWidget* parent, bool browsable, QFileDialog::FileMode mode ):
  CustomDialog( parent )
{ 

  if( browsable ) 
  {
    
    BrowsedLineEditor* browse_edit = new BrowsedLineEditor( this );
    mainLayout().addWidget( browse_edit );
    browse_edit->setFileMode( mode );
    editor_ = &browse_edit->editor();
    
  } else {
    
    editor_ = new BrowsedLineEditor::Editor( this );
    mainLayout().addWidget( &editor() );
  
  }
  
  return;
}

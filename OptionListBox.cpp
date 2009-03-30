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


#include "Debug.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "LineEditor.h"
#include "OptionListBox.h"
#include "TextEditionDelegate.h"
#include "TreeView.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________________
OptionListBox::OptionListBox( QWidget* parent, const QString& name ):
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
  _list().setMask( 1<<OptionModel::CURRENT|1<<OptionModel::VALUE );
  _list().setItemDelegate( new TextEditionDelegate( this ) );
  _list().setIconSize( IconSize( IconSize::SMALL ) );
  layout->addWidget( &_list(), 1 );
  
  QVBoxLayout* button_layout = new QVBoxLayout();
  button_layout->setMargin(0);
  button_layout->setSpacing( 5 );
  layout->addLayout( button_layout, 0 );

  // Add button
  QPushButton *button;
  button_layout->addWidget( button = new QPushButton( "&Add", this ) );
  button->setIcon( IconEngine::get( ICONS::ADD ) );
  connect( button, SIGNAL( clicked() ), SLOT( _add() ) );
 
  // Add action
  QAction* action;
  addAction( action = new QAction( IconEngine::get( ICONS::ADD ), "&Add", this ) );
  connect( action, SIGNAL( triggered() ), SLOT( _add() ) );
  action->setShortcut( Qt::CTRL + Qt::Key_N );
  _list().menu().addAction( action );
  
  // remove button
  button_layout->addWidget( remove_ = new QPushButton( "&Remove", this ) );
  connect( remove_, SIGNAL( clicked() ), SLOT( _remove() ) );
  remove_->setIcon( IconEngine::get( ICONS::REMOVE ) );
  remove_->setToolTip( "Remove selected value" );

  addAction( remove_action_ = new QAction( IconEngine::get( ICONS::REMOVE ), "Remo&ve", this ) );
  connect( remove_action_, SIGNAL( triggered() ), SLOT( _remove() ) );
  remove_action_->setShortcut( Qt::Key_Delete );
  remove_action_->setToolTip( "Remove selected value" );  
  _list().menu().addAction( remove_action_ );

  // Edit button
  button_layout->addWidget( edit_ = new QPushButton( "&Edit", this ) );
  connect( edit_, SIGNAL( clicked() ), SLOT( _edit() ) );
  edit_->setIcon( IconEngine::get( ICONS::RENAME ) );
  edit_->setToolTip( "Edit selected value" );

  addAction( edit_action_ = new QAction( IconEngine::get( ICONS::RENAME ),  "&Edit", this ) );
  connect( edit_action_, SIGNAL( triggered() ), SLOT( _edit() ) );
  _list().menu().addAction( edit_action_ );
  
  // set default button
  button_layout->addWidget( default_ = new QPushButton( "&Default", this ) );
  connect( default_, SIGNAL( clicked() ), SLOT( _setDefault() ) );
  default_->setToolTip( "Set selected value as default\n(move it to the top of the list)" );
  default_->setIcon( IconEngine::get( ICONS::DIALOG_OK_APPLY ) );
  
  addAction( default_action_ = new QAction( IconEngine::get( ICONS::DIALOG_OK_APPLY ), "&Default", this ) );
  connect( default_action_, SIGNAL( triggered() ), SLOT( _setDefault() ) );
  _list().menu().addAction( default_action_ );

  button_layout->addStretch(1);
      
  // set connections
  connect( _list().selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ), SLOT( _updateButtons() ) );
  connect( _list().selectionModel(), SIGNAL( currentChanged( const QModelIndex& , const QModelIndex& ) ), SLOT( _updateButtons() ) );
  connect( &_list(), SIGNAL( activated( const QModelIndex& ) ), SLOT( _setDefault() ) );

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
  if( !values.empty() && find_if( values.begin(), values.end(), Option::HasFlagFTor( Option::CURRENT ) ) == values.end() )
  { values.front().setCurrent( true ); }
  
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
  edit_action_->setEnabled( current.isValid() && model_.get( current ).second.hasFlag( Option::RECORDABLE ) );

  default_->setEnabled( current.isValid() );
  default_action_->setEnabled( current.isValid() );

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
  dialog.checkbox().setChecked( true );
  
  if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
  if( dialog.editor().text().isEmpty() ) return;
  
  // create new item
  Options::Pair option( optionName(), dialog.editor().text() );
  model_.add( option );
  
  // make sure item is selected
  QModelIndex index( model_.index( option ) );
  if( index != _list().selectionModel()->currentIndex() )
  {
    _list().selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    _list().selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
  }
  
  // update default state
  if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
  { _setDefault(); }

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
  dialog.editor().setText( option.second.raw() );
  if( option.second.isCurrent() ) 
  {
    dialog.checkbox().setChecked( true );
    dialog.checkbox().setEnabled( false );
  }
  
  // map dialog
  if( dialog.centerOnParent().exec() == QDialog::Rejected ) return;
  if( dialog.editor().text().isEmpty() ) return;
  
  // first remove old option
  model_.remove( option );

  // modify value 
  option.second.setRaw( dialog.editor().text() );
  
  // re-add to model
  model_.add( option );
  
  // make sure item is selected
  QModelIndex index( model_.index( option ) );
  if( index != _list().selectionModel()->currentIndex() )
  {
    _list().selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    _list().selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
  }  
  
  // update default state
  if( dialog.checkbox().isEnabled() && dialog.checkbox().isChecked() )
  { _setDefault(); }
  
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
  { iter->second.setCurrent( false ); }
  
  model_.set( OptionModel::Set( options.begin(), options.end() ) );
  
  Options::Pair current_option( model_.get( current ) );
  current_option.second.setCurrent( true );
  model_.add( current_option );

  _list().resizeColumns();

}

//_______________________________________________________
OptionListBox::EditDialog::EditDialog( QWidget* parent, bool browsable, QFileDialog::FileMode mode ):
  CustomDialog( parent )
{ 

  setOptionName( "OPTIONLISTBOX_EDIT" );
  QVBoxLayout* v_layout = new QVBoxLayout();
  v_layout->setMargin(0);
  v_layout->setSpacing(5);
  mainLayout().addLayout( v_layout );
  
  if( browsable ) 
  {
    
    BrowsedLineEditor* browse_edit = new BrowsedLineEditor( this );
    v_layout->addWidget( browse_edit );
    browse_edit->setFileMode( mode );
    editor_ = &browse_edit->editor();
    
  } else {
    
    editor_ = new BrowsedLineEditor::Editor( this );
    v_layout->addWidget( &editor() );
  
  }
  
  v_layout->addWidget( checkbox_ = new QCheckBox( "Set as default", this ) );
  
  setMinimumSize( QSize( 320, 0 ) );

  return;
}

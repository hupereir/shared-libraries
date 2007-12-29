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
   \file    HelpDialog.cpp
   \brief   reference manual display dialog 
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QAction>
#include <QLayout>
#include <QPushButton>
#include <QShortcut>
#include <QHeaderView>

#include "CustomTextEdit.h"
#include "HelpDialog.h"
#include "HelpDelegate.h"
#include "HelpManager.h"
#include "HelpModel.h"
#include "NewItemDialog.h"
#include "QtUtil.h"
#include "TreeView.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;
using namespace BASE;

//_________________________________________________________
HelpDialog::HelpDialog( QWidget *parent ):
  QDialog( parent, Qt::Window ),
  Counter( "HelpDialog" )
{

  Debug::Throw( "HelpDialog::HelpDialog.\n" );
  
  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );
  
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setSpacing(5);
  layout->setMargin(10);
  setLayout( layout );
  
  // add help list
  list_ = new TreeView( this );
  list_->setMaximumWidth(150);
  layout->addWidget( list_ );
  list_->setModel( &model_ );
  list_->setItemDelegate( new HelpDelegate( this ) );
  list_->setSortingEnabled( false );
  list_->header()->hide();
   
  // stack widget to switch between html and plain text editor
  layout->addLayout( stack_layout_ = new QStackedLayout() );
  
  // add html editor
  stack_layout_->addWidget( html_frame_ = new QWidget( this ) );
  
  // vbox layout for editor and button
  QVBoxLayout *v_layout = new QVBoxLayout();
  v_layout->setMargin(0);
  v_layout->setSpacing( 5 );
  html_frame_->setLayout( v_layout );
  
  v_layout->addWidget( html_editor_ = new CustomTextEdit( html_frame_ ) );
  html_editor_->setReadOnly( true );
  html_editor_->setWrapFromOptions( false );
  html_editor_->wrapModeAction().setChecked( true );
  
  QFont font;
  font.fromString( XmlOptions::get().raw( "FONT_NAME" ).c_str() );
  html_editor_->setFont( font );
  
  Debug::Throw( "HelpDialog::HelpDialog - html editor done.\n" );
  
  // button layout
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setSpacing( 10 );
  h_layout->setMargin(0);
  h_layout->setDirection( QBoxLayout::RightToLeft );  
  v_layout->addLayout( h_layout );

  QPushButton* button;
  h_layout->addWidget( button = new QPushButton( "&Close", html_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setToolTip( "Close the reference manual window" );
  
  h_layout->addWidget( edit_button_ = new QPushButton( "&Edit", html_frame_ ) );  
  connect( edit_button_, SIGNAL( clicked() ), SLOT( _toggleEdition() ) );
  edit_button_->setToolTip( "edit current help" );
  h_layout->addStretch( 1 );
  Debug::Throw( "HelpDialog::HelpDialog - html button frame done.\n" );

  // add plain editor
  stack_layout_->addWidget( plain_frame_ = new QWidget( this ) );

  // vbox layout for editor and button
  v_layout = new QVBoxLayout();
  v_layout->setMargin(0);
  v_layout->setSpacing( 5 );
  plain_frame_->setLayout( v_layout );

  // plain editor
  v_layout->addWidget( plain_editor_ = new CustomTextEdit( plain_frame_ ) );
  plain_editor_->setReadOnly( false );
  plain_editor_->setWrapFromOptions( false );
  plain_editor_->wrapModeAction().setChecked( true );
  
  // button layout
  h_layout = new QHBoxLayout();
  h_layout->setSpacing( 10 );
  h_layout->setMargin(0);
  h_layout->setDirection( QBoxLayout::RightToLeft );  
  v_layout->addLayout( h_layout );

  h_layout->addWidget( button = new QPushButton( "&Close", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setToolTip( "Close the reference manual window" );
  
  h_layout->addWidget( button = new QPushButton( "&End edition", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _toggleEdition() ) );
  button->setToolTip( "edit current help" );

  h_layout->addWidget( button = new QPushButton( "&Delete item", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _deleteItem() ) );
  button->setToolTip( "delete current help item" );

  h_layout->addWidget( button = new QPushButton( "&Add item", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _newItem() ) );
  button->setToolTip( "add a new help item" );
  h_layout->addStretch( 1 );

  // make sure html edition is visible at start up  
  stack_layout_->setCurrentWidget( html_frame_ );
    
  // connect list to text edit
  connect( list_->selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( _display( const QModelIndex&, const QModelIndex& ) ) );
  connect( &model_, SIGNAL( itemMoved( int ) ), SLOT( _moveItem( int ) ) );
  connect( &model_, SIGNAL( itemRenamed( QModelIndex, QString ) ), SLOT( _renameItem( QModelIndex, QString ) ) );

  // add close accelerator
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  connect( new QShortcut( CTRL+Key_S, this ), SIGNAL( activated() ), SLOT( _updateItemFromEditor() ) );
  resize( 700, 650 );
  
}

//_________________________________________________________
void HelpDialog::setItems( const HelpItem::List& items )
{

  Debug::Throw( "HelpDialog::setItems.\n" );
  
  // clear list and editors
  html_editor_->clear();
  plain_editor_->clear();
  
  model_.set( HelpModel::List( items.begin(), items.end() ) );
  
  return;

}

//_________________________________________________________
void HelpDialog::addItem( const HelpItem& item )
{

  Debug::Throw( "HelpDialog::AddItem.\n" );
  model_.add( item );
  return;

}

//_________________________________________________________
void HelpDialog::closeEvent( QCloseEvent *e )
{
  Debug::Throw( "HelpDialog::closeEvent" );
  _updateItemFromEditor();  
}

//_________________________________________________________
void HelpDialog::_display( const QModelIndex& current, const QModelIndex& previous )
{

  Debug::Throw( "HelpDialog::_Display.\n" );
  
  // save modifications to current item, if needed
  if( model_.editionEnabled() && previous.isValid() && current != previous ) 
  { _updateItemFromEditor( previous ); }
  
  // check validity
  if( !current.isValid() ) 
  {
    
    // clear editors
    html_editor_->clear();
    plain_editor_->clear();

  } else {
  
    // retrieve item
    const HelpItem& item( model_.get( current ) );

    // update editors
    if( plain_editor_->toPlainText() != item.text().c_str() )
    {
      html_editor_->setHtml( item.text().c_str() );
      plain_editor_->setPlainText( item.text().c_str() );
    }
    
  }
  
  return;

}

//_________________________________________________________
void HelpDialog::_updateItemFromEditor( QModelIndex index, bool forced )
{
  Debug::Throw( "HelpDialog::_updateItemFromEditor.\n" );
 
  // current index
  if( !index.isValid() ) index = list_->selectionModel()->currentIndex();
  
  // update current item text if being edited
  if( model_.editionEnabled() && index.isValid() )
  { 
    HelpItem item( model_.get( index ) );
    bool modified = !(item.text() == qPrintable( plain_editor_->toPlainText() ) );
    if( forced || modified ) 
    {
      item.setText( qPrintable( plain_editor_->toPlainText() ) );
      model_.replace( index, item );
      _updateHelpManager();
    }
  }
}

//_________________________________________________________
void HelpDialog::_updateHelpManager( void )
{
 
  Debug::Throw( "HelpDialog::_updateHelpManager.\n" );
  
  // retrieve all texts, pass to help manager
  const HelpModel::List& model_list( model_.get() );
  HelpManager::install( HelpItem::List( model_list.begin(), model_list.end() ) );
  HelpManager::setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_toggleEdition( void )
{

  Debug::Throw( "HelpDialog::_ToggleEdition.\n" );  
  
  // current index
  QModelIndex current( list_->selectionModel()->currentIndex() );
  
  if( !model_.editionEnabled() )
  {
    
    // backup help manager
    HelpManager::backup();
    model_.setEditionEnabled( true );
    list_->setDragEnabled(true);
    list_->setAcceptDrops(true);
   
    // modify current item display
    if( current.isValid() ) plain_editor_->setPlainText( model_.get(current).text().c_str() );
    stack_layout_->setCurrentWidget( plain_frame_ );
    
  } else {

    // save modifications to current item, if needed
    if( current.isValid() ) _updateItemFromEditor( current );
    model_.setEditionEnabled( false );
    list_->setDragEnabled(false);
    list_->setAcceptDrops(false);
    _askForSave();

    if( current.isValid() ) html_editor_->setHtml( model_.get(current).text().c_str() );
    stack_layout_->setCurrentWidget( html_frame_ );
    
  }

  return;

}

//_________________________________________________________
void HelpDialog::_moveItem( int row )
{
  Debug::Throw( "HelpDialog::_moveItem.\n" );
  QModelIndex current( list_->selectionModel()->currentIndex() );
  if( !current.isValid() ) return;
  HelpItem item( model_.get( current ) );
  
  // add at last position if row is not valid
  if( row < 0 || row >= model_.rowCount() ) {

    // clear selection and current index
    list_->clearSelection();
    list_->selectionModel()->setCurrentIndex( QModelIndex(), QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

    // remove old position item
    model_.remove( item );
    model_.add( item );
    
    // select last row
    QModelIndex index( model_.index( model_.rowCount()-1, 0 ) );
    list_->selectionModel()->select( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    list_->selectionModel()->setCurrentIndex( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

  } else if( row == current.row() ) return;
  else {

    // clear selection and current index
    list_->clearSelection();
    list_->selectionModel()->setCurrentIndex( QModelIndex(), QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

    // remove old position item
    model_.remove( item );
    
    // update row if needed
    if( current.row() < row ) row--;
    
    // re-add item at new position
    QModelIndex index( model_.index( row, 0 ) );
    model_.insert( index, item );
   
    // set new index as selected
    list_->selectionModel()->select( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    list_->selectionModel()->setCurrentIndex( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    
  } 
  
  // set manager as modified
  HelpManager::setModified( true );
  _updateHelpManager();
  return;
  
}

//_________________________________________________________
void HelpDialog::_renameItem( QModelIndex index, QString value )
{
  Debug::Throw( "HelpDialog::_renameItem.\n" );
  if( !index.isValid() || value.isNull() || value.isEmpty() ) return;
  HelpItem item( model_.get( index ) );
  string new_label( qPrintable( value ) );
  if( new_label != item.label() )
  {
    item.setLabel( qPrintable( value ) );
    model_.replace( index, item );
    HelpManager::setModified( true );
    _updateHelpManager();
  }
  return;  
}

//_________________________________________________________
void HelpDialog::_newItem( void )
{
  Debug::Throw( "HelpDialog::_newItem.\n" );
  NewItemDialog dialog( this );
  if( dialog.exec() == QDialog::Rejected ) return;
  
  // retrieve item name
  string item_name( dialog.itemName() );
  if( item_name.empty() ) return;
  model_.add( HelpItem( item_name, "" ) );
  HelpManager::setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_deleteItem( void )
{
  
  Debug::Throw( "HelpDialog::_deleteItem.\n" );

  // current index
  QModelIndex current( list_->selectionModel()->currentIndex() );
  if( !current.isValid() ) return;

  if( !QtUtil::questionDialog( this, "Delete current help item ?" ) ) return;
  model_.remove( model_.get( current ) );
  html_editor_->clear();
  plain_editor_->clear();
  HelpManager::setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_askForSave( void )
{
  Debug::Throw( "HelpDialog::_askForSave.\n" );
  
  // double check that modifications are permanent
  if( HelpManager::modified() && !( QtUtil::questionDialog( this, "Help has been modified. Save ?" ) ) )
  {
    
    HelpManager::restoreBackup();
    stack_layout_->setCurrentWidget( html_frame_ );
    setItems( HelpManager::items() );
    
  } 
  
}

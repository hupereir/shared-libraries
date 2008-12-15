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
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QShortcut>
#include <QTextStream>

#include "BaseIcons.h"
#include "IconEngine.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "TextEditionDelegate.h"
#include "HelpManager.h"
#include "HelpModel.h"
#include "NewItemDialog.h"
#include "QuestionDialog.h"
#include "TextEditor.h"
#include "TreeView.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;
using namespace BASE;

//_________________________________________________________
HelpDialog::HelpDialog( HelpManager& manager, QWidget *parent ):
  BaseDialog( parent, Qt::Window ),
  Counter( "HelpDialog" ),
  manager_( &manager )
{

  Debug::Throw( "HelpDialog::HelpDialog.\n" );
  
  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );

  setSizeOptionName( "HELP_DIALOG" );
  
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setSpacing(5);
  layout->setMargin(10);
  setLayout( layout );
  
  // add help list
  list_ = new TreeView( this );
  _list().setMaximumWidth(150);
  layout->addWidget( list_ );
  _list().setModel( &model_ );
  _list().setItemDelegate( new TextEditionDelegate( this ) );
  _list().setSortingEnabled( false );
  _list().header()->hide();
   
  // stack widget to switch between html and plain text editor
  layout->addLayout( stack_layout_ = new QStackedLayout() );
  
  // add html editor
  stack_layout_->addWidget( html_frame_ = new QWidget( this ) );
  
  // vbox layout for editor and button
  QVBoxLayout *v_layout = new QVBoxLayout();
  v_layout->setMargin(0);
  v_layout->setSpacing( 5 );
  html_frame_->setLayout( v_layout );
  
  v_layout->addWidget( html_editor_ = new TextEditor( html_frame_ ) );
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
  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Close", html_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setToolTip( "Close the reference manual window" );
  
  h_layout->addWidget( edit_button_ = new QPushButton( IconEngine::get( ICONS::EDIT ), "&Edit", html_frame_ ) );  
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
  v_layout->addWidget( plain_editor_ = new TextEditor( plain_frame_ ) );
  plain_editor_->setReadOnly( false );
  plain_editor_->setWrapFromOptions( false );
  plain_editor_->wrapModeAction().setChecked( true );
  
  // button layout
  h_layout = new QHBoxLayout();
  h_layout->setSpacing( 10 );
  h_layout->setMargin(0);
  h_layout->setDirection( QBoxLayout::RightToLeft );  
  v_layout->addLayout( h_layout );

  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Close", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setToolTip( "Close the reference manual window" );
  
  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_OK ), "&Apply", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _toggleEdition() ) );
  button->setToolTip( "edit current help" );

  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::FIND ), "&Show help string", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _showHelpString() ) );

  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::REMOVE ), "&Delete item", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _deleteItem() ) );
  button->setToolTip( "delete current help item" );

  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::ADD ), "&Add item", plain_frame_ ) );  
  connect( button, SIGNAL( clicked() ), SLOT( _newItem() ) );
  button->setToolTip( "add a new help item" );
  h_layout->addStretch( 1 );

  // make sure html edition is visible at start up  
  stack_layout_->setCurrentWidget( html_frame_ );
    
  // connect list to text edit
  connect( _list().selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( _display( const QModelIndex&, const QModelIndex& ) ) );
  connect( &model_, SIGNAL( itemMoved( int ) ), SLOT( _moveItem( int ) ) );
  connect( &model_, SIGNAL( itemRenamed( QModelIndex, QString ) ), SLOT( _renameItem( QModelIndex, QString ) ) );

  // add close accelerator
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  connect( new QShortcut( CTRL+Key_S, this ), SIGNAL( activated() ), SLOT( _updateItemFromEditor() ) );
  
}

//_________________________________________________________
void HelpDialog::setItems( const HelpItem::List& items )
{

  Debug::Throw( "HelpDialog::setItems.\n" );
  
  // clear list and editors
  html_editor_->clear();
  plain_editor_->clear();
  
  // set items 
  _model().set( HelpModel::List( items.begin(), items.end() ) );
  
  // select first index
  if( (!_list().selectionModel()->currentIndex().isValid()) && _model().hasIndex(0,0) ) 
  { _list().selectionModel()->setCurrentIndex( _model().index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }
  
  
  return;

}

//_________________________________________________________
void HelpDialog::addItem( const HelpItem& item )
{

  Debug::Throw( "HelpDialog::AddItem.\n" );
  _model().add( item );
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
  if( _model().editionEnabled() && previous.isValid() && current != previous ) 
  { _updateItemFromEditor( previous ); }
  
  // check validity
  if( !current.isValid() ) 
  {
    
    // clear editors
    html_editor_->clear();
    plain_editor_->clear();

  } else {
  
    // retrieve item
    const HelpItem& item( _model().get( current ) );

    // update editors
    if( plain_editor_->toPlainText() != item.text() )
    {
      html_editor_->setHtml( item.text() );
      plain_editor_->setPlainText( item.text() );
    }
    
  }
  
  return;

}

//_________________________________________________________
void HelpDialog::_updateItemFromEditor( QModelIndex index, bool forced )
{
  Debug::Throw( "HelpDialog::_updateItemFromEditor.\n" );
 
  // current index
  if( !index.isValid() ) index = _list().selectionModel()->currentIndex();
  
  // update current item text if being edited
  if( _model().editionEnabled() && index.isValid() )
  { 
    HelpItem item( _model().get( index ) );
    bool modified = !(item.text() == qPrintable( plain_editor_->toPlainText() ) );
    if( forced || modified ) 
    {
      item.setText( qPrintable( plain_editor_->toPlainText() ) );
      _model().replace( index, item );
      _updateHelpManager();
    }
  }
}

//_________________________________________________________
void HelpDialog::_updateHelpManager( void )
{
 
  Debug::Throw( "HelpDialog::_updateHelpManager.\n" );
  
  // retrieve all texts, pass to help manager
  const HelpModel::List& model_list( _model().get() );
  _manager().install( HelpItem::List( model_list.begin(), model_list.end() ) );
  _manager().setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_toggleEdition( void )
{

  Debug::Throw( "HelpDialog::_ToggleEdition.\n" );  
  
  // current index
  QModelIndex current( _list().selectionModel()->currentIndex() );
  
  if( !_model().editionEnabled() )
  {
    
    // backup help manager
    _manager().backup();
    _model().setEditionEnabled( true );
    _list().setDragEnabled(true);
    _list().setAcceptDrops(true);
   
    // modify current item display
    if( current.isValid() ) plain_editor_->setPlainText( _model().get(current).text() );
    stack_layout_->setCurrentWidget( plain_frame_ );
    
  } else {

    // save modifications to current item, if needed
    if( current.isValid() ) _updateItemFromEditor( current );
    _model().setEditionEnabled( false );
    _list().setDragEnabled(false);
    _list().setAcceptDrops(false);
    _askForSave();

    if( current.isValid() ) html_editor_->setHtml( _model().get(current).text() );
    stack_layout_->setCurrentWidget( html_frame_ );
    
  }

  return;

}

//_________________________________________________________
void HelpDialog::_moveItem( int row )
{
  Debug::Throw( "HelpDialog::_moveItem.\n" );
  QModelIndex current( _list().selectionModel()->currentIndex() );
  if( !current.isValid() ) return;
  HelpItem item( _model().get( current ) );
  
  // add at last position if row is not valid
  if( row < 0 || row >= _model().rowCount() ) {

    // clear selection and current index
    _list().clearSelection();
    _list().selectionModel()->setCurrentIndex( QModelIndex(), QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

    // remove old position item
    _model().remove( item );
    _model().add( item );
    
    // select last row
    QModelIndex index( _model().index( _model().rowCount()-1, 0 ) );
    _list().selectionModel()->select( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    _list().selectionModel()->setCurrentIndex( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

  } else if( row == current.row() ) return;
  else {

    // clear selection and current index
    _list().clearSelection();
    _list().selectionModel()->setCurrentIndex( QModelIndex(), QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );

    // remove old position item
    _model().remove( item );
    
    // update row if needed
    if( current.row() < row ) row--;
    
    // re-add item at new position
    QModelIndex index( _model().index( row, 0 ) );
    _model().insert( index, item );
   
    // set new index as selected
    _list().selectionModel()->select( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    _list().selectionModel()->setCurrentIndex( index, QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows );
    
  } 
  
  // set manager as modified
  _manager().setModified( true );
  _updateHelpManager();
  return;
  
}

//_________________________________________________________
void HelpDialog::_renameItem( QModelIndex index, QString value )
{
  Debug::Throw( "HelpDialog::_renameItem.\n" );
  if( !index.isValid() || value.isNull() || value.isEmpty() ) return;
  HelpItem item( _model().get( index ) );
  if( value != item.label() )
  {
    item.setLabel( value );
    _model().replace( index, item );
    _manager().setModified( true );
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
  QString item_name( dialog.itemName() );
  if( item_name.isEmpty() ) return;
  _model().add( HelpItem( item_name, "" ) );
  _manager().setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_deleteItem( void )
{
  
  Debug::Throw( "HelpDialog::_deleteItem.\n" );

  // current index
  QModelIndex current( _list().selectionModel()->currentIndex() );
  if( !current.isValid() ) return;

  if( !QuestionDialog( this, "Delete current help item ?" ).exec() ) return;
  _model().remove( _model().get( current ) );
  html_editor_->clear();
  plain_editor_->clear();
  _manager().setModified( true );
  
}

//_________________________________________________________
void HelpDialog::_showHelpString( void )
{
  Debug::Throw( "HelpDialog::_showHelpString.\n" );

  
  // write output to stream
  QString buffer;
  QTextStream stream( &buffer );
  
  // retrieve all items from dialog
  stream << "static const char* HelpText[] = {\n";
  HelpItem::List items( _model().get() );
  for( HelpItem::List::const_iterator iter = items.begin(); iter != items.end(); iter++ )
  {
    
    // dump label
    stream << "  //_________________________________________________________\n"; 
    stream << "  \"" << iter->label() << "\",\n";
    
    // dump text
    QString text( iter->text() );
    text.replace( "\"", "\\\"" );
    text.replace( "\n", "\\n\"\n  \"" );
    stream << "  \"" << text << "\"";
    stream << ",\n";
    stream << "\n";
  }
  stream << "  0\n";
  stream << "};\n";
  
  CustomDialog dialog( 0, CustomDialog::OK_BUTTON );
  TextEditor* editor( new TextEditor( &dialog ) );

  editor->setWrapFromOptions( false );
  editor->wrapModeAction().setChecked( false );
  editor->setPlainText( buffer );
  dialog.mainLayout().addWidget( editor );
  dialog.resize( 600, 500 );
  
  // center 
  dialog.centerOnWidget( qApp->activeWindow() ).exec();

}

//_________________________________________________________
void HelpDialog::_askForSave( void )
{
  Debug::Throw( "HelpDialog::_askForSave.\n" );
  
  // double check that modifications are permanent
  if( _manager().modified() && !( QuestionDialog( this, "Help has been modified. Save ?" ).exec() ) )
  {
    
    _manager().restoreBackup();
    stack_layout_->setCurrentWidget( html_frame_ );
    setItems( _manager().items() );
    
  } 
  
}

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

#include "CustomTextEdit.h"
#include "HelpDialog.h"
#include "HelpManager.h"
#include "NewItemDialog.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;
using namespace BASE;

//_________________________________________________________
HelpDialog::HelpDialog( QWidget *parent ):
  QDialog( parent, Qt::Window ),
  Counter( "HelpDialog" ),
  edited_( false ),
  current_item_( 0 )
{

  Debug::Throw( "HelpDialog::HelpDialog.\n" );
  
  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );
  
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setSpacing(5);
  layout->setMargin(10);
  setLayout( layout );
  
  // add help list
  list_ = new HelpItemList( this );
  list_->setMaximumWidth(150);
  // list_->setMovement(QListView::Static);
  layout->addWidget( list_ );
    
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
  
  h_layout->addWidget( button = new QPushButton( "&Save", plain_frame_ ) );  
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
  connect( &list(), SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) ), SLOT( _display( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
  //connect( &list(), SIGNAL( itemSelectionChanged() ), SLOT( _display() ) );

  // add close accelerator
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  connect( new QShortcut( CTRL+Key_S, this ), SIGNAL( activated() ), SLOT( _save() ) );
  resize( 700, 650 );
}

//_________________________________________________________
void HelpDialog::addItem( const HelpItem& item )
{

  Debug::Throw( "HelpDialog::AddItem.\n" );
  new HelpItemList::Item( &list(), item );
  return;

}

//_________________________________________________________
void HelpDialog::closeEvent( QCloseEvent *e )
{
  Debug::Throw( "HelpDialog::closeEvent" );
  _save();  
}

//_________________________________________________________
void HelpDialog::_display( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{

  Debug::Throw( "HelpDialog::_Display.\n" );
  
  // save modifications to current item, if needed
  if( previous && current != previous ) _save();

  HelpItemList::Item* item( dynamic_cast<HelpItemList::Item*>( current ) );
  if( !item ) return;

  current_item_ = item;
  
  html_editor_->setHtml( current_item_->item().text().c_str() );
  plain_editor_->setPlainText( current_item_->item().text().c_str() );
  
  return;

}

//_________________________________________________________
void HelpDialog::_save( bool forced )
{
  Debug::Throw( "HelpDialog::_Save.\n" );

  // update current item text if being edited
  if( edited_ && current_item_ )
  { 
    bool modified = !(current_item_->item().text() == qPrintable( plain_editor_->toPlainText() ) );
    if( forced || modified )
    {
      current_item_->item().setText( qPrintable( plain_editor_->toPlainText() ) );
      
      // retrieve all texts, pass to help manager
      HelpManager::List items;
      QList<HelpItemList::Item*> list_items( list().children<HelpItemList::Item>() );
      for( QList<HelpItemList::Item*>::iterator iter = list_items.begin(); iter != list_items.end(); iter++ )
      { items.push_back( (*iter)->item() ); }
      
      HelpManager::install( items );
      HelpManager::setModified( true );
      
    }
  }
  
}

//_________________________________________________________
void HelpDialog::_toggleEdition( void )
{

  Debug::Throw( "HelpDialog::_ToggleEdition.\n" );
  if( !current_item_ ) return;
  
  if( !edited_ )
  {
    
    edited_ = true;
    plain_editor_->setPlainText( current_item_->item().text().c_str() );
    stack_layout_->setCurrentWidget( plain_frame_ );
    
  } else {

    // save modifications to current item, if needed
    _save();
  
    edited_ = false;
    html_editor_->setHtml( current_item_->item().text().c_str() );
    stack_layout_->setCurrentWidget( html_frame_ );
    
  }

  // enable/disable drag in keyword list
  list().setDragEnabled( edited_ );

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

  HelpItemList::Item* item  = new HelpItemList::Item( &list(), HelpItem( item_name, "" ) );
  list().setCurrentItem( item );
  list().setItemSelected( item, true );
  
}

//_________________________________________________________
void HelpDialog::_deleteItem( void )
{
  Debug::Throw( "HelpDialog::_deleteItem.\n" );
  if( !current_item_ ) return;
  if( !QtUtil::questionDialog( this, "Delete current help item ?" ) ) return;
  delete current_item_;
  _save( true );
}

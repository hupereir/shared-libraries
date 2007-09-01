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
   \file    HelpDialog.cc
   \brief   reference manual display dialog 
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QAction>
#include <QLayout>
#include <QPushButton>
#include <QShortcut>

#include "HelpDialog.h"
#include "HelpManager.h"
#include "CustomTextEdit.h"
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

  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );
  
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setSpacing(5);
  layout->setMargin(10);
  setLayout( layout );
  
  // add help list
  list_ = new HelpItemList( this );
  list_->setMaximumWidth(150);
  list_->setMovement(QListView::Static);
  layout->addWidget( list_ );
  
  // vbox layout for editor and button
  QVBoxLayout *v_layout( new QVBoxLayout() );
  v_layout->setMargin(0);
  v_layout->setSpacing( 5 );
  layout->addLayout( v_layout );
  
  // add text editor
  v_layout->addWidget( text_edit_ = new CustomTextEdit( this ) );
  text_edit_->setReadOnly( true );
  text_edit_->setWrapFromOptions( false );
  text_edit_->wrapModeAction().setChecked( true );
  
  QFont font;
  font.fromString( XmlOptions::get().raw( "FONT_NAME" ).c_str() );
  text_edit_->setFont( font );
  
  // button layout
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setSpacing( 10 );
  h_layout->setMargin(0);
  h_layout->setDirection( QBoxLayout::RightToLeft );  
  v_layout->addLayout( h_layout );

  QPushButton* button;
  h_layout->addWidget( button = new QPushButton( "&Close", this ) );  
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setToolTip( "Close the reference manual window" );
  
  
  h_layout->addWidget( edit_button_ = new QPushButton( "&Edit", this ) );  
  connect( edit_button_, SIGNAL( clicked() ), SLOT( _toggleEdition() ) );
  edit_button_->setToolTip( "edit current help" );
  h_layout->addStretch( 1 );
  
  // connect list to text edit
  connect( &list(), SIGNAL( itemSelectionChanged() ), SLOT( _display() ) );

  // add close accelerator
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  connect( new QShortcut( CTRL+Key_S, this ), SIGNAL( activated() ), SLOT( _save() ) );
  resize( 700, 650 );
}

//_________________________________________________________
void HelpDialog::addItem( const HelpItem& item )
{

  Debug::Throw( "HelpDialog::AddItem.\n" );
  new HelpItemList::Item( list_, item );
  return;

}

//_________________________________________________________
void HelpDialog::closeEvent( QCloseEvent *e )
{
  Debug::Throw( "HelpDialog::closeEvent" );
  _save();  
}

//_________________________________________________________
void HelpDialog::_display()
{

  Debug::Throw( "HelpDialog::_Display.\n" );
  
  // save modifications to current item, if needed
  _save();
  
  QList<HelpItemList::Item*> items( list().selectedItems<HelpItemList::Item>() );
  if( items.empty() ) return;
  
  current_item_ = items.front();
  if( edited_ ) text_edit_->setPlainText( current_item_->item().text().c_str() );
  else text_edit_->setHtml( current_item_->item().text().c_str() );
  
  return;

}

//_________________________________________________________
void HelpDialog::_save( void )
{
  Debug::Throw( "HelpDialog::_Save.\n" );

  // update current item text if being edited
  if( edited_ && current_item_ )
  { 
    bool modified = !(current_item_->item().text() == qPrintable( text_edit_->toPlainText() ) );
    if( modified ) 
    {
      current_item_->item().setText( qPrintable( text_edit_->toPlainText() ) );
      
      // retrieve all texts, pass to help manager
      HelpManager::List items;
      QList<HelpItemList::Item*> list_items( list().items<HelpItemList::Item>() );
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
    edit_button_->setText( "&Save" );
    QFont font;
    font.fromString( XmlOptions::get().raw( "FIXED_FONT_NAME" ).c_str() );
    text_edit_->setFont( font );
    text_edit_->setCurrentFont( font );
    text_edit_->setPlainText( current_item_->item().text().c_str() );
    text_edit_->setReadOnly( false );
  
  } else {

    // save modifications to current item, if needed
    _save();
  
    edited_ = false;
    edit_button_->setText( "&Edit" );
    text_edit_->setReadOnly( true );
    
    // update editor
    text_edit_->clear();
    QFont font;
    font.fromString( XmlOptions::get().raw( "FONT_NAME" ).c_str() );
    text_edit_->setFont( font );
    text_edit_->setCurrentFont( font );
    text_edit_->setHtml( current_item_->item().text().c_str() );
    
  }

  return;

}


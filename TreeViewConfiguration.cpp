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
  \file TreeViewConfiguration.cpp
  \brief Configuration vbox for TreeView columns
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

#include "Exception.h"
#include "TreeViewConfiguration.h"
#include "QtUtil.h"
#include "Debug.h"

using namespace std;
using namespace Qt;

//____________________________________________________________________________
TreeViewConfiguration::TreeViewConfiguration( QWidget *parent, QTreeView *target, const string& title ):
    QWidget( parent ),
    Counter( "TreeViewConfiguration" ),
    target_( target ),
    modified_mask_( 0 ),
    backup_mask_( 0 )
{

  Debug::Throw( "TreeViewConfiguration::TreeViewConfiguration.\n" ); 
  
  // adjust flags
  if( !parent ) {
    setAttribute( WA_DeleteOnClose );
    setWindowModality( ApplicationModal );
  }
  
  // create vbox layout
  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setMargin(0);
  layout->setSpacing(2);
  
  if( !isTopLevel() ) 
  layout->setSpacing( 5 );
  setLayout( layout );
  
  // check target
  Exception::checkPointer( target_, DESCRIPTION("invalid target_") );
  
  // size
  QCheckBox *checkbox;
  QGroupBox *box = new QGroupBox( title.c_str(), this );
  layout->addWidget( box );

  QVBoxLayout* box_layout( new QVBoxLayout() );
  box_layout->setMargin(5);
  box_layout->setSpacing(2);
  box->setLayout( box_layout );
    
  // retrieve columns
  QHeaderView* header( target_->header() );
  Exception::checkPointer( header, DESCRIPTION( "invalid header" ) );
  for( int index=0; index < header->count(); index++ )
  {
    
    // retrieve column name
    QString column_name( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );    
    if( column_name.isNull() || column_name.isEmpty() )
    {
      ostringstream what;
      what << "column " << index+1;
      column_name = what.str().c_str();
    }
    
    // add checkbox
    checkbox = new QCheckBox( column_name, box );
    box_layout->addWidget( checkbox );
    
    checkbox->setChecked( !target_->isColumnHidden( index ) );
    checkbox_.push_back( checkbox );
    
    // add tooltip
    ostringstream what;
    what << "show/hide column \"" << qPrintable( column_name ) << "\"";
    checkbox->setToolTip( what.str().c_str() );
    
  }

  // compute mask according to button state
  modified_mask_ = backup_mask_ = _computeMask();
        
  // add ok/cancel button if no parent
  if( !parent )
  {
    QHBoxLayout *button_layout( new QHBoxLayout() );
    button_layout->setMargin( 5 );
    button_layout->setSpacing( 5 );
    layout->addLayout( button_layout );
    
    // ok button
    QPushButton *button = new QPushButton( "&Ok", this );
    QtUtil::fixSize( button );
    connect( button, SIGNAL( clicked() ), SLOT( update() ) );
    connect( button, SIGNAL( clicked() ), SLOT( close() ) );
    button->setAutoDefault( false );
    button_layout->addWidget( button );
    
    // cancel button
    button = new QPushButton( "&Cancel", this );
    QtUtil::fixSize( button );
    connect( button, SIGNAL( clicked() ), SLOT( restore() ) );
    connect( button, SIGNAL( clicked() ), SLOT( close() ) );
    button->setAutoDefault( false );  
    button_layout->addWidget( button );
    
  }
  
} 

//____________________________________________________________________________
void TreeViewConfiguration::restore( void )
{
  Debug::Throw( "TreeViewConfiguration::restore.\n" ); 
  
  // set check button state according to the backup mask
  for( unsigned int index = 0; index < checkbox_.size(); index++ )
  checkbox_[index]->setChecked( backup_mask_ & (1<<index) );
  
  // update
  update();
  
  return;
  
}

//____________________________________________________________________________
void TreeViewConfiguration::update( void )
{
  Debug::Throw( "TreeViewConfiguration::update.\n" ); 
  bool modified( false );
  
  for( unsigned int index = 0; index < checkbox_.size(); index++ )
  {
    if( !_modified( index ) ) continue;
    modified = true;
    target_->setColumnHidden( index, !checkbox_[index]->isChecked() );
  }
  
  if( modified ) target_->repaint();
  
  // update mask
  modified_mask_ = _computeMask();
  
  return;
  
}

//____________________________________________________________________________
unsigned int TreeViewConfiguration::_computeMask( void ) const
{

  Debug::Throw( "TreeViewConfiguration::_computeMask.\n" );
  unsigned int out = 0;
  for( unsigned int index = 0; index < checkbox_.size(); index ++ )
  if( checkbox_[index]->isChecked() ) out |= (1 << index );
  return out;

}

//____________________________________________________________________________
bool TreeViewConfiguration::_modified( const unsigned int & index ) const
{
  Debug::Throw( "TreeViewConfiguration::_modified.\n" ); 
  Exception::check( index < checkbox_.size(), DESCRIPTION( "invalid index" ) );
  
  // get initial state from mask
  bool initial_state( modified_mask_ & (1 << index ) );
  bool final_state( checkbox_[index]->isChecked() );
  return initial_state != final_state;
}

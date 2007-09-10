
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
  \file ListViewConfiguration.cc
  \brief Configuration vbox for ListView columns
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QGroupBox>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

#include "Exception.h"
#include "ListViewConfiguration.h"
#include "QtUtil.h"
#include "Debug.h"

using namespace std;
using namespace Qt;

//____________________________________________________________________________
ListViewConfiguration::ListViewConfiguration( QWidget *parent, QTreeWidget *target, const string& title ):
    QWidget( parent ),
    Counter( "ListViewConfiguration" ),
    target_( target ),
    modified_mask_( 0 ),
    backup_mask_( 0 )
{

  Debug::Throw( "ListViewConfiguration::ListViewConfiguration.\n" ); 
  
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
  for( int index=0; index < target_->columnCount(); index++ )
  {
    
    // retrieve column name
    QTreeWidgetItem* header( target_->headerItem() );
    
    string column_name("");
    if( header ) column_name = qPrintable( target_->headerItem()->text( index ) );
    if( column_name.empty() )
    {
      
      ostringstream what;
      what << "Column " << index + 1;
      column_name = what.str();
      
    }
    
    Debug::Throw() << "ListViewConfiguration::ListViewConfiguration - column: " << column_name << endl;
    
    // add checkbox
    checkbox = new QCheckBox( column_name.c_str(), box );
    box_layout->addWidget( checkbox );
    
    checkbox->setChecked( !target_->isColumnHidden( index ) );
    checkbox_.push_back( checkbox );
    
    // add tooltip
    ostringstream what;
    what << "show/hide column \"" << column_name << "\"";
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
void ListViewConfiguration::restore( void )
{
  Debug::Throw( "ListViewConfiguration::restore.\n" ); 
  
  // set check button state according to the backup mask
  for( unsigned int index = 0; index < checkbox_.size(); index++ )
  checkbox_[index]->setChecked( backup_mask_ & (1<<index) );
  
  // update
  update();
  
  return;
  
}

//____________________________________________________________________________
void ListViewConfiguration::update( void )
{
  Debug::Throw( "ListViewConfiguration::update.\n" ); 
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
unsigned int ListViewConfiguration::_computeMask( void ) const
{

  Debug::Throw( "ListViewConfiguration::_computeMask.\n" );
  unsigned int out = 0;
  for( unsigned int index = 0; index < checkbox_.size(); index ++ )
  if( checkbox_[index]->isChecked() ) out |= (1 << index );
  return out;

}

//____________________________________________________________________________
bool ListViewConfiguration::_modified( const unsigned int & index ) const
{
  Debug::Throw( "ListViewConfiguration::_modified.\n" ); 
  Exception::check( index < checkbox_.size(), DESCRIPTION( "invalid index" ) );
  
  // get initial state from mask
  bool initial_state( modified_mask_ & (1 << index ) );
  bool final_state( checkbox_[index]->isChecked() );
  return initial_state != final_state;
}

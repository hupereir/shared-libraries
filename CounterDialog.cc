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
 * ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or            
 * FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License            
 * for more details.                        
 *                             
 * You should have received a copy of the GNU General Public License along with 
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple       
 * Place, Suite 330, Boston, MA   02111-1307 USA                                      
 *                            
 *                            
 *******************************************************************************/
/*!
   \file CounterDialog.cc
   \brief displays Counter names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QLayout>

#include "CounterDialog.h"
#include "CounterList.h"
#include "Str.h"

using namespace std;

//__________________________________________________________________________
CounterDialog::CounterDialog( QWidget* parent ):
  QDialog( parent, Qt::Window ),
  Counter( "CounterDialog" )
{
  
  Debug::Throw( "CounterDialog::CounterDialog.\n" );
  setWindowTitle( "object counters" );
    
  setLayout( new QVBoxLayout() );
  layout()->setMargin(10);
  layout()->setSpacing(10);
  
  // insert main vertical box
  counter_list_ = new CounterList( this );
  counter_list_->setLineWidth( 1 );
  counter_list_->setMidLineWidth( 0 );
  
  layout()->addWidget( counter_list_ );
  
  // update button
  QPushButton *update_button = new QPushButton( "&Update", this );
  connect( update_button, SIGNAL( clicked() ), this, SLOT( Update() ) );
  update_button->setAutoDefault( false );
  layout()->addWidget( update_button );
   
  resize( QSize( 200,300 ) );
}

//__________________________________________________________________________
void CounterDialog::Update( void )
{
  
  Debug::Throw( "CounterDialog::Update.\n" );
    
  // retrieve counters 
  Counter::CounterMap& counters( Counter::map() );

  for( Counter::CounterMap::const_iterator it=counters.begin(); it != counters.end(); it++ )
  {
    
    std::map<string, CustomListView::Item* >::iterator item_it( items_.find( it->first ) );
    CustomListView::Item *item( 0 );
    if( item_it == items_.end() ) {
      item = new CustomListView::Item( counter_list_ );
      items_.insert( make_pair( it->first, item ) );
    } else item = item_it->second;
    
    item->setText( CounterList::NAME, it->first.c_str() );
    item->setText( CounterList::COUNT, Str().assign<int>(it->second).c_str() );
  
  }
  
}

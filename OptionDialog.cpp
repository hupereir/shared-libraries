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
   \file OptionDialog.cpp
   \brief displays all run-time options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QProcess>
#include <QLayout>

#include "OptionDialog.h"
#include "CustomListView.h"
#include "Str.h"
#include "XmlOptions.h"

using namespace std;

//__________________________________________________________________________
OptionDialog::OptionDialog( QWidget* parent ):
  QDialog( parent, Qt::Window ),
  Counter( "OptionDialog" )
{
  
  Debug::Throw( "OptionDialog::OptionDialog.\n" );
  setWindowTitle( "object counters" );
    
  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );
  
  setLayout( new QVBoxLayout() );
  layout()->setMargin(10);
  layout()->setSpacing(10);
  
  // insert list
  CustomListView* list = new CustomListView( this );
  layout()->addWidget( list );
  list->setColumnCount(2);
  list->setColumnName(0, "Name" );
  list->setColumnName(1, "Value" );
  
  // retrieve all special options
  const Options::SpecialOptionMap special_options( XmlOptions::get().specialOptions() );
  for( Options::SpecialOptionMap::const_iterator iter = special_options.begin(); iter != special_options.end(); iter++ )
  {
    for(  Options::OptionList::const_iterator option_iter = iter->second.begin(); option_iter != iter->second.end(); option_iter++ )
    {
      // create new item
      CustomListView::Item* item = new CustomListView::Item( list );
      item->setText( 0, iter->first.c_str() );
      item->setText( 1, option_iter->raw().c_str() );
      item->setColor( "#0000ff" );
    }
  }
  
  // retrieve normal options
  const Options::OptionMap options( XmlOptions::get().options() );
  for( Options::OptionMap::const_iterator iter = options.begin(); iter != options.end(); iter++ )
  {
    // create new item
    CustomListView::Item* item = new CustomListView::Item( list );
    item->setText( 0, iter->first.c_str() );
    item->setText( 1, iter->second.raw().c_str() );
  }
  
  list->resizeColumnToContents( 1 );
  list->resizeColumnToContents( 0 );
  list->sortItems(0, Qt::AscendingOrder );
  
  QPushButton *button;
  layout()->addWidget( button = new QPushButton( "&Close", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setAutoDefault( false );
  
  resize( QSize( 400,300 ) );
  
}

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
   \file SystemEnvironmentDialog.cpp
   \brief displays system environment
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QProcess>
#include <QLayout>

#include "SystemEnvironmentDialog.h"
#include "CustomListView.h"
#include "Str.h"

using namespace std;

//__________________________________________________________________________
SystemEnvironmentDialog::SystemEnvironmentDialog( QWidget* parent ):
  QDialog( parent, Qt::Window ),
  Counter( "SystemEnvironmentDialog" )
{
  
  Debug::Throw( "SystemEnvironmentDialog::SystemEnvironmentDialog.\n" );
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
  
  // retrieve environment variables from QProcess
  QStringList env( QProcess::systemEnvironment() );
  for( QStringList::iterator iter = env.begin(); iter != env.end(); iter++ )
  {
    QStringList parsed( (iter)->split( "=" ) );
    if( parsed.empty() ) continue;
    CustomListView::Item* item = new CustomListView::Item();
    item->setText( 0, parsed[0] );
    if( parsed.size() > 1 ) item->setText( 1, parsed[1] );
    list->addTopLevelItem( item );
  }
  
  list->resizeColumnToContents( 1 );
  list->resizeColumnToContents( 0 );
  list->sortItems(0, Qt::AscendingOrder );
  
  QPushButton *button;
  layout()->addWidget( button = new QPushButton( "&Close", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setAutoDefault( false );
  
  resize( QSize( 600,300 ) );
  
}

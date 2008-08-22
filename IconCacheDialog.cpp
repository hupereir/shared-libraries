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
   \file IconCacheDialog.cpp
   \brief displays IconCache names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QLayout>

#include "BaseIcons.h"
#include "IconCacheDialog.h"
#include "IconEngine.h"
#include "TreeView.h"

using namespace std;

//__________________________________________________________________________
IconCacheDialog::IconCacheDialog( QWidget* parent ):
  BaseDialog( parent, Qt::Window ),
  Counter( "IconCacheDialog" )
{
  
  Debug::Throw( "IconCacheDialog::IconCacheDialog.\n" );
  setWindowTitle( "icon cache" );
  _setSizeOptionName( "ICON_CACHE_DIALOG" );
  
  setLayout( new QVBoxLayout() );
  layout()->setMargin(10);
  layout()->setSpacing(10);
  
  // insert main vertical box
  list_ = new TreeView( this );  
  layout()->addWidget( list_ );
  list_->setModel( &model_ );  
  list_->setIconSizeFromOptions( false );
  list_->setIconSize( QSize( 32, 32 ) );
  
  // update button
  QHBoxLayout *h_layout = new QHBoxLayout();
  h_layout->setSpacing(10);
  h_layout->setMargin(0);
  layout()->addItem( h_layout );
  
  QPushButton *button;
  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::RELOAD ),"&Update", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( update() ) );
  button->setAutoDefault( false );
   
  h_layout->addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ),"&Close", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  button->setAutoDefault( false );
    
}

//__________________________________________________________________________
void IconCacheDialog::update( void )
{
  
  Debug::Throw( "IconCacheDialog::update.\n" );
    
  // retrieve cache 
  const IconEngine::Cache& cache( IconEngine::cache() );
  model_.set( IconCacheModel::List( cache.begin(), cache.end() ) );

  list_->resizeColumnToContents( IconCacheModel::ICON );
    
}

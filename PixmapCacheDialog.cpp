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
   \file PixmapCacheDialog.cpp
   \brief displays PixmapCache names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QLayout>

#include "BaseIcons.h"
#include "CustomPixmap.h"
#include "PixmapCacheDialog.h"
#include "PixmapEngine.h"
#include "IconSize.h"
#include "AnimatedTreeView.h"

using namespace std;

//__________________________________________________________________________
PixmapCacheDialog::PixmapCacheDialog( QWidget* parent ):
  CustomDialog( parent, OkButton, Qt::Window )
{
  
  Debug::Throw( "PixmapCacheDialog::PixmapCacheDialog.\n" );
  setWindowTitle( "icon cache" );
  setOptionName( "ICON_CACHE_DIALOG" );
    
  // insert main vertical box
  list_ = new AnimatedTreeView( this );  
  mainLayout().addWidget( list_ );
  list_->setModel( &model_ );  
  list_->setIconSize( IconSize( IconSize::LARGE ) );
    
  QPushButton *button;
  buttonLayout().insertWidget( 1, button = new QPushButton( PixmapEngine::get( ICONS::RELOAD ),"&Update", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( update() ) );
  button->setAutoDefault( false );
   
}

//__________________________________________________________________________
void PixmapCacheDialog::update( void )
{
  
  Debug::Throw( "PixmapCacheDialog::update.\n" );
    
  // retrieve cache 
  const PixmapEngine::Cache& cache( PixmapEngine::cache() );
  model_.set( PixmapCacheModel::List( cache.begin(), cache.end() ) );

  list_->resizeColumnToContents( PixmapCacheModel::ICON );
    
}

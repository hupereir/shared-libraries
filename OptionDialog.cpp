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

#include "OptionItemDelegate.h"
#include "OptionDialog.h"
#include "TreeView.h"
#include "Str.h"
#include "XmlOptions.h"

using namespace std;

//__________________________________________________________________________
OptionDialog::OptionDialog( QWidget* parent ):
  CustomDialog( parent, CustomDialog::OK_BUTTON )
{
  
  Debug::Throw( "OptionDialog::OptionDialog.\n" );
  setWindowTitle( "object counters" );
  _setSizeOptionName( "OPTION_DIALOG" );
    
  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );

  // set model editable
  model_.setReadOnly( false );
  
  // insert list
  TreeView* list = new TreeView( this );
  mainLayout().addWidget( list );
  list->setModel( &model_ );
  list->setRootIsDecorated( true );
  list->setItemDelegate( new OptionItemDelegate( this ) );
  
  // retrieve all special options
  const Options::SpecialMap special_options( XmlOptions::get().specialOptions() );
  for( Options::SpecialMap::const_iterator iter = special_options.begin(); iter != special_options.end(); iter++ )
  { 
    model_.add( OptionPair( iter->first, "" ) );
    OptionModel::List options;
    for( Options::List::const_iterator list_iter = iter->second.begin(); list_iter != iter->second.end(); list_iter++ )
    { model_.add( OptionPair( iter->first, *list_iter ) ); }
    
  }
  
  // retrieve normal options
  const Options::Map& options( XmlOptions::get().options() );
  OptionModel::Set option_set;
  for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); iter++ )
  { option_set.insert( OptionPair( iter->first, iter->second ) ); }
  model_.add( option_set );
  
  list->resizeColumns();

}

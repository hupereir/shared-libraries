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
  \file CustomDialog.cpp
  \brief customized QDialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "BaseIcons.h"
#include "CustomDialog.h"
#include "IconEngine.h"

using namespace std;

//____________________________________________________________
CustomDialog::CustomDialog( QWidget *parent, const unsigned int& flags, Qt::WFlags wflags):
  BaseDialog( parent, wflags ),
  Counter( "CustomDialog" ),
  ok_button_( 0 ),
  cancel_button_( 0 )
{

  Debug::Throw( "CustomDialog::CustomDialog.\n" );
  
  // create vbox layout
  QVBoxLayout* layout( new QVBoxLayout() );
  setLayout( layout );
  layout->setSpacing(10);
  layout->setMargin(10);
  
  main_layout_ = new QVBoxLayout();
  main_layout_->setSpacing(10);
  main_layout_->setMargin(0);
  layout->addLayout( main_layout_, 1 );
    
  // insert hbox layout for buttons
  button_layout_ = new QBoxLayout( QBoxLayout::LeftToRight );
  button_layout_->setSpacing(10);
  button_layout_->setMargin(0);
  layout->addLayout( button_layout_, 0 );
  
  // insert OK and Cancel button
  if( flags & OK_BUTTON ) {
    button_layout_->addWidget( ok_button_ = new QPushButton( IconEngine::get( ICONS::DIALOG_ACCEPT ), "&OK", this ) );
    connect( ok_button_, SIGNAL( clicked() ), SLOT( accept() ) );
  }
  
  // insert OK and Cancel button
  if( flags & CANCEL_BUTTON ) {
    button_layout_->addWidget( cancel_button_ = new QPushButton( IconEngine::get( ICONS::DIALOG_CLOSE ), "&Cancel", this ) );
    connect( cancel_button_, SIGNAL( clicked() ), SLOT( reject() ) );
  }
  
}

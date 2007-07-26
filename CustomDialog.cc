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
  \file CustomDialog.cc
  \brief customized QDialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomDialog.h"
#include "QtUtil.h"

using namespace std;

//____________________________________________________________
CustomDialog::CustomDialog( 
  QWidget *parent, 
  const unsigned int& flags, 
  Qt::WFlags wflags):
  QDialog( parent, wflags ),
  Counter( "CustomDialog" ),
  ok_button_( 0 ),
  cancel_button_( 0 )
{

  Debug::Throw( "CustomDialog::CustomDialog.\n" );
  
  // create vbox layout
  QVBoxLayout* layout( new QVBoxLayout() );
  setLayout( layout );
  layout->setSpacing(2);
  layout->setMargin(2);
  
  // insert main vertical box
  main_widget_ = new QWidget( this );
  main_widget_->setLayout( new QVBoxLayout() );
  main_widget_->layout()->setMargin( 5 );
  main_widget_->layout()->setSpacing( 5 );
  layout->addWidget( main_widget_ );
  
  // insert hbox layout for buttons
  button_layout_ = new QBoxLayout( QBoxLayout::LeftToRight );
  button_layout_->setSpacing(5);
  button_layout_->setMargin(5);
  layout->addLayout( button_layout_ );
  
  // insert OK and Cancel button
  if( flags & OK_BUTTON ) {
    ok_button_ = new QPushButton( "&OK", this );
    QtUtil::fixSize( ok_button_ );
    connect( ok_button_, SIGNAL( clicked() ), this, SLOT( accept() ) );
    button_layout_->addWidget( ok_button_ );
  }
  
  // insert OK and Cancel button
  if( flags & CANCEL_BUTTON ) {
    cancel_button_ = new QPushButton( "&Cancel", this );
    QtUtil::fixSize( cancel_button_ );
    connect( cancel_button_, SIGNAL( clicked() ), this, SLOT( reject() ) );
    button_layout_->addWidget( cancel_button_ );
  }
  
  layout->activate();
  
}

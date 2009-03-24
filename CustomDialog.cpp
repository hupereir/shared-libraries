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

#include <QFrame>

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
  layout->setSpacing(5);
  layout->setMargin(10);
  
  main_layout_ = new QVBoxLayout();
  main_layout_->setSpacing(5);
  main_layout_->setMargin(0);
  layout->addLayout( main_layout_, 1 );

  // separator
  if( flags & SEPARATOR )
  {
    QFrame* frame( new QFrame( this ) );
    frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    layout->addWidget( frame );
  }
  
  // insert hbox layout for buttons
  button_layout_ = new QBoxLayout( QBoxLayout::LeftToRight );
  button_layout_->setSpacing(5);
  button_layout_->setMargin(0);
  layout->addLayout( button_layout_, 0 );
  
  button_layout_->addStretch( 1 );
  
  // insert OK and Cancel button
  if( flags & OK_BUTTON ) {
    QIcon icon( IconEngine::get( ( flags & CANCEL_BUTTON ) ? ICONS::DIALOG_OK : ICONS::DIALOG_CLOSE ) );
    QString text( ( flags & CANCEL_BUTTON ) ? "&Ok":"&Close" );
    button_layout_->addWidget( ok_button_ = new QPushButton( icon, text, this ) );
    connect( ok_button_, SIGNAL( clicked() ), SLOT( accept() ) );
    ok_button_->setDefault( true );
  }
  
  // insert OK and Cancel button
  if( flags & CANCEL_BUTTON ) {
    button_layout_->addWidget( cancel_button_ = new QPushButton( IconEngine::get( ICONS::DIALOG_CANCEL ), "&Cancel", this ) );
    connect( cancel_button_, SIGNAL( clicked() ), SLOT( reject() ) );
  }
  
}

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



//____________________________________________________________
CustomDialog::CustomDialog( QWidget *parent, const unsigned int& flags, Qt::WFlags wflags):
  BaseDialog( parent, wflags ),
  Counter( "CustomDialog" ),
  OkButton_( 0 ),
  CancelButton_( 0 )
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
  if( flags & Separator )
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
  if( flags & OkButton ) {
    QIcon icon( IconEngine::get( ( flags & CancelButton ) ? ICONS::DIALOG_OK : ICONS::DIALOG_CLOSE ) );
    QString text( ( flags & CancelButton ) ? "&Ok":"&Close" );
    button_layout_->addWidget( OkButton_ = new QPushButton( icon, text, this ) );
    connect( OkButton_, SIGNAL( clicked() ), SLOT( accept() ) );
    OkButton_->setDefault( true );
  }

  // insert OK and Cancel button
  if( flags & CancelButton ) {
    button_layout_->addWidget( CancelButton_ = new QPushButton( IconEngine::get( ICONS::DIALOG_CANCEL ), "&Cancel", this ) );
    connect( CancelButton_, SIGNAL( clicked() ), SLOT( reject() ) );
  }

}

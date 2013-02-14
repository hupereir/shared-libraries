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
*******************************************************************************/

#include "BaseIcons.h"
#include "CustomDialog.h"
#include "IconEngine.h"

#include <QFrame>

//____________________________________________________________
CustomDialog::CustomDialog( QWidget *parent, Flags flags, Qt::WindowFlags wflags):
    BaseDialog( parent, wflags ),
    Counter( "CustomDialog" ),
    separator_( 0 ),
    okButton_( 0 ),
    cancelButton_( 0 )
{

    Debug::Throw( "CustomDialog::CustomDialog.\n" );

    // create vbox layout
    QVBoxLayout* layout( new QVBoxLayout() );
    setLayout( layout );
    layout->setSpacing(5);
    layout->setMargin(10);

    mainLayout_ = new QVBoxLayout();
    mainLayout_->setSpacing(5);
    mainLayout_->setMargin(0);
    layout->addLayout( mainLayout_, 1 );

    // separator
    if( flags & Separator )
    {
        QFrame* frame( new QFrame( this ) );
        frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
        layout->addWidget( frame );
        separator_ = frame;
    }

    // insert hbox layout for buttons
    buttonLayout_ = new QBoxLayout( QBoxLayout::LeftToRight );
    buttonLayout_->setSpacing(5);
    buttonLayout_->setMargin(0);
    layout->addLayout( buttonLayout_, 0 );

    buttonLayout_->addStretch( 1 );

    // insert OK and Cancel button
    if( flags & OkButton )
    {
        QIcon icon( IconEngine::get( ( flags & CancelButton ) ? ICONS::DIALOG_OK : ICONS::DIALOG_CLOSE ) );
        QString text( ( flags & CancelButton ) ? "Ok":"Close" );
        buttonLayout_->addWidget( okButton_ = new QPushButton( icon, text, this ) );
        connect( okButton_, SIGNAL( clicked() ), SLOT( accept() ) );
        okButton_->setDefault( true );
    }

    // insert Cancel button
    if( flags & CancelButton )
    {
        buttonLayout_->addWidget( cancelButton_ = new QPushButton( IconEngine::get( ICONS::DIALOG_CANCEL ), "Cancel", this ) );
        connect( cancelButton_, SIGNAL( clicked() ), SLOT( reject() ) );
    }

}

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "BaseIconNames.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "Debug.h"
#include "BaseFindDialog.h"

#include <QFrame>
#include <QPushButton>

//________________________________________________________________________
BaseFindDialog::BaseFindDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseDialog( parent, flags ),
    Counter( "BaseFindDialog" )
{
    Debug::Throw( "BaseFindDialog::BaseFindDialog.\n" );
    setOptionName( "FIND_DIALOG" );

    // set dialog title
    setWindowTitle( tr( "Find" ) );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin( 10 );
    layout()->setSpacing( 5 );

    // base find widget
    layout()->addWidget( baseFindWidget_ = new BaseFindWidget( this ) );
    connect( baseFindWidget_, SIGNAL(find(TextSelection)), this, SIGNAL(find(TextSelection)) );

    // horizontal separator
    QFrame* frame( new QFrame( this ) );
    frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    layout()->addWidget( frame );

    // buttons
    layout()->addItem( buttonLayout_ = new QHBoxLayout() );
    buttonLayout_->setMargin( 0 );
    buttonLayout_->setSpacing( 5 );
    buttonLayout_->addStretch(1);

    // insert Find button
    QPushButton *button;
    buttonLayout_->addWidget( button = new QPushButton( IconEngine::get( IconNames::Find ), tr( "Find" ), this ) );
    connect( button, SIGNAL(clicked()), SLOT(_find()) );
    connect( button, SIGNAL(clicked()), SLOT(_updateFindComboBox()) );
    _addDisabledButton( button );
    button->setAutoDefault( false );
    findButton_ = button;

    // insert Cancel button
    buttonLayout_->addWidget( button = new QPushButton( IconEngine::get( IconNames::DialogClose ), tr( "Close" ), this ) );
    connect( button, SIGNAL(clicked()), SLOT(close()) );
    button->setShortcut( Qt::Key_Escape );
    button->setAutoDefault( false );

}

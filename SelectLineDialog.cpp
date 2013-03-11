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

#include "SelectLineDialog.h"

#include "BaseIcons.h"
#include "IconEngine.h"

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QValidator>

//_______________________________________________________
SelectLineDialog::SelectLineDialog( QWidget* parent, Qt::WindowFlags flags ):
    CustomDialog( parent, OkButton|CancelButton, flags )
{

    Debug::Throw( "SelectLineDialog::SelectLineDialog.\n" );

    // set dialog title
    setWindowTitle( tr( "Goto Line Number" ) );

    // insert text editor
    QLabel *label( new QLabel( tr( "Goto line number:" ), this ) );
    mainLayout().addWidget( label, 0 );

    mainLayout().addWidget( editor_ = new AnimatedLineEditor( this ), 1 );
    label->setBuddy( editor_ );

    //connect( editor(), SIGNAL(returnPressed()), SLOT( _selectLine( void ) ) );
    connect( editor_, SIGNAL(returnPressed( void ) ), SLOT( _selectLine( void ) ) );
    connect( editor_, SIGNAL(textChanged( const QString& ) ), SLOT( _selectLine( void ) ) );

    QIntValidator *validator = new QIntValidator( this );
    validator->setBottom(0);
    editor_->setValidator( validator );

    // connections
    connect( &okButton(), SIGNAL( clicked( void ) ), SLOT( _selectLine( void ) ) );

    // minimum size
    setMinimumSize( QSize( 250, 100 ) );

}

//_______________________________________________________
void SelectLineDialog::_selectLine( void )
{ emit lineSelected( Str( editor_->text() ).get<int>()-1 ); }

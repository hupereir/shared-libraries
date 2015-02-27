
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

#include "SelectLineDialog.h"
#include "Debug.h"

#include <QLayout>

//_______________________________________________________
SelectLineDialog::SelectLineDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseDialog( parent, flags ),
    Counter( "SelectLineDialog" )
{

    Debug::Throw( "SelectLineDialog::SelectLineDialog.\n" );

    // set dialog title
    setWindowTitle( tr( "Goto Line Number" ) );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin( 10 );
    layout()->setSpacing( 5 );

    setSelectLineWidget( new SelectLineWidget( this, false ) );

    // minimum size
    setMinimumSize( QSize( 250, 100 ) );
}

//________________________________________________________________________
void SelectLineDialog::setSelectLineWidget( SelectLineWidget* selectLineWidget )
{
    Debug::Throw( "BaseFindDialog::setBaseFindWidget.\n" );
    if( selectLineWidget_ )
    {
        selectLineWidget_->hide();
        selectLineWidget_->deleteLater();
    }

    // assign new widget and change parent
    selectLineWidget_ = selectLineWidget;
    if( selectLineWidget_->parent() != this )
    {
        selectLineWidget_->setParent( this );
        selectLineWidget_->show();
    }

    // append to layout
    layout()->addWidget( selectLineWidget_ );

    // setup connections
    connect( selectLineWidget_, SIGNAL(lineSelected(int)), this, SIGNAL(lineSelected(int)) );
    connect( &selectLineWidget_->okButton(), SIGNAL(clicked()), SLOT(close()));
    connect( &selectLineWidget_->closeButton(), SIGNAL(clicked()), SLOT(close()));

}

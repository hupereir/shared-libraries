
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

#include "BaseFindDialog.h"

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

    setBaseFindWidget( new BaseFindWidget( this, false ) );
}

//________________________________________________________________________
void BaseFindDialog::setBaseFindWidget( BaseFindWidget* baseFindWidget )
{
    Debug::Throw( "BaseFindDialog::setBaseFindWidget.\n" );
    if( baseFindWidget_ )
    {
        baseFindWidget_->hide();
        baseFindWidget_->deleteLater();
    }

    // assign new widget and change parent
    baseFindWidget_ = baseFindWidget;
    if( baseFindWidget_->parent() != this )
    {
        baseFindWidget_->setParent( this );
        baseFindWidget_->show();
    }

    // append to layout
    layout()->addWidget( baseFindWidget_ );

    // setup connections
    connect( baseFindWidget_, SIGNAL(find(TextSelection)), this, SIGNAL(find(TextSelection)) );
    connect( &baseFindWidget_->closeButton(), SIGNAL(clicked()), SLOT(close()) );

}

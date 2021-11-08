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
#include "BaseFindWidget.h"
#include "QtUtil.h"


#include <QLayout>

//________________________________________________________________________
BaseFindDialog::BaseFindDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseDialog( parent, flags ),
    Counter( QStringLiteral("BaseFindDialog") )
{
    Debug::Throw( QStringLiteral("BaseFindDialog::BaseFindDialog.\n") );
    setOptionName( QStringLiteral("FIND_DIALOG") );

    // set dialog title
    setWindowTitle( tr( "Find" ) );

    // create vbox layout
    setLayout( new QVBoxLayout );
    QtUtil::setMargin(layout(), 10);
    layout()->setSpacing( 5 );

    setBaseFindWidget( new BaseFindWidget( this, false ) );
}

//________________________________________________________________________
void BaseFindDialog::setBaseFindWidget( AbstractFindWidget* baseFindWidget )
{
    Debug::Throw( QStringLiteral("BaseFindDialog::setBaseFindWidget.\n") );

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
    connect( baseFindWidget_, &AbstractFindWidget::find, this, &BaseFindDialog::find );
    connect( &baseFindWidget_->closeButton(), &QAbstractButton::clicked, this, &QWidget::close );

}

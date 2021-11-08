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

#include "Debug.h"
#include "QtUtil.h"
#include "SelectLineDialog.h"
#include "SelectLineWidget.h"

#include <QLayout>

//_______________________________________________________
SelectLineDialog::SelectLineDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseDialog( parent, flags ),
    Counter( QStringLiteral("SelectLineDialog") )
{

    Debug::Throw( QStringLiteral("SelectLineDialog::SelectLineDialog.\n") );

    // set dialog title
    setWindowTitle( tr( "Goto Line Number" ) );

    // create vbox layout
    setLayout( new QVBoxLayout );
    QtUtil::setMargin(layout(), 10);
    layout()->setSpacing( 5 );

    setSelectLineWidget( new SelectLineWidget( this, false ) );

    // minimum size
    setMinimumSize( QSize( 250, 100 ) );
}

//________________________________________________________________________
SelectLineDialog::~SelectLineDialog() = default;

//________________________________________________________________________
LineEditor& SelectLineDialog::editor() const
{ return selectLineWidget_->editor(); }

//________________________________________________________________________
void SelectLineDialog::setSelectLineWidget( SelectLineWidget* selectLineWidget )
{
    Debug::Throw( QStringLiteral("BaseFindDialog::setBaseFindWidget.\n") );

    // assign new widget and change parent
    selectLineWidget_.reset( selectLineWidget );
    if( selectLineWidget_->parent() != this )
    {
        selectLineWidget_->setParent( this );
        selectLineWidget_->show();
    }

    // append to layout
    layout()->addWidget( selectLineWidget_.get() );

    // setup connections
    connect( selectLineWidget_.get(), &SelectLineWidget::lineSelected, this, &SelectLineDialog::lineSelected );
    connect( &selectLineWidget_->okButton(), &QAbstractButton::clicked, this, &QWidget::close);
    connect( &selectLineWidget_->closeButton(), &QAbstractButton::clicked, this, &QWidget::close);

}

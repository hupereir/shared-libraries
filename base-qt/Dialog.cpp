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
#include "Dialog.h"
#include "IconEngine.h"
#include "QtUtil.h"

#include <QFrame>

//____________________________________________________________
Dialog::Dialog( QWidget *parent, Flags flags, Qt::WindowFlags wflags):
    BaseDialog( parent, wflags ),
    Counter( QStringLiteral("Dialog") )
{

    Debug::Throw( QStringLiteral("Dialog::Dialog.\n") );

    // create vbox layout
    auto layout = new QVBoxLayout;
    setLayout( layout );
    layout->setSpacing(5);
    QtUtil::setMargin(layout, 10);

    mainLayout_ = new QVBoxLayout;
    mainLayout_->setSpacing(5);
    QtUtil::setMargin(mainLayout_, 0);
    layout->addLayout( mainLayout_, 1 );

    // separator
    if( flags & Separator )
    {
        auto frame = new QFrame( this );
        frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
        layout->addWidget( frame );
        separator_ = frame;
    }

    QDialogButtonBox::StandardButtons buttons;
    if( flags& OkButton ) buttons |= (flags&CancelButton) ? QDialogButtonBox::Ok : QDialogButtonBox::Close;
    if( flags& CancelButton ) buttons |= QDialogButtonBox::Cancel;

    buttonBox_ = new QDialogButtonBox( buttons, Qt::Horizontal, this );
    layout->addWidget( buttonBox_ );

    // update icons, on windows
    #if defined(Q_OS_WIN)
    if( hasCancelButton() ) cancelButton().setIcon( IconEngine::get( IconNames::DialogCancel ) );
    if( hasOkButton() ) okButton().setIcon( hasCancelButton() ? IconEngine::get( IconNames::DialogOk ) : IconEngine::get( IconNames::DialogClose ) );
    #endif

    connect( buttonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( buttonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject );

}

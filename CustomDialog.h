#ifndef CustomDialog_h
#define CustomDialog_h

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

#include "BaseDialog.h"
#include "Counter.h"
#include "Debug.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QLayout>

//* customized QDialog
class CustomDialog: public BaseDialog, public Counter
{

    Q_OBJECT

    public:

    //* mask used to define number of buttons in customized dialogs
    enum Flag
    {
        None = 0,
        OkButton = 1<<0,
        CloseButton = 1<<0,
        CancelButton = 1<<1,
        Separator = 1<<2
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //* constructor
    CustomDialog(
        QWidget* = 0x0,
        Flags = Flags( OkButton | CancelButton ),
        Qt::WindowFlags = 0);

    //* destructor
    virtual ~CustomDialog( void )
    {}

    //* retrieve main vbox
    QBoxLayout& mainLayout( void ) const
    { return *mainLayout_; }

    //* returns true if OK button is valid
    bool hasSeparator( void ) const
    { return separator_; }

    //* separator
    QWidget& separator( void ) const
    { return *separator_; }

    //* retrieve button layout
    QBoxLayout& buttonLayout( void ) const
    { return *(qobject_cast<QBoxLayout*>(buttonBox_->layout())); }

    //* returns true if OK button is valid
    bool hasOkButton( void ) const
    { return buttonBox_->button(QDialogButtonBox::Ok)||buttonBox_->button(QDialogButtonBox::Close); }

    //* retrieve OK button
    QPushButton& okButton( void ) const
    {
        if( QPushButton* button = buttonBox_->button(QDialogButtonBox::Ok) ) return *button;
        else return *buttonBox_->button(QDialogButtonBox::Close);
    }

    //* returns true if close button is valid
    bool hasCloseButton( void ) const
    { return hasOkButton(); }

    //* retrieve close button
    QPushButton& closeButton( void ) const
    { return okButton(); }

    //* returns true if Cancel button is valid
    bool hasCancelButton( void ) const
    { return buttonBox_->button(QDialogButtonBox::Cancel); }

    //* retrieve CANCEL button
    QPushButton& cancelButton( void ) const
    { return *buttonBox_->button(QDialogButtonBox::Cancel); }

    private:

    //* main layout
    QBoxLayout *mainLayout_ = nullptr;

    //* separator if any
    QWidget* separator_ = nullptr;

    //* dialog button box
    QDialogButtonBox* buttonBox_ = nullptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( CustomDialog::Flags );

#endif

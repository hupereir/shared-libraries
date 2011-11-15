#ifndef CustomDialog_h
#define CustomDialog_h

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

#include "BaseDialog.h"
#include "Counter.h"
#include "Debug.h"

#include <cassert>
#include <QtGui/QPushButton>
#include <QtGui/QBoxLayout>
#include <QtGui/QLayout>

//! customized QDialog
class CustomDialog: public BaseDialog, public Counter
{

    public:

    //! mask used to define number of buttons in customized dialogs
    enum Flag
    {
        //! no flags
        None = 0,

        //! dialog has OK button
        OkButton = 1<<0,

        //! dialog has CANCEL button
        CancelButton = 1<<1,

        //! separator
        Separator = 1<<2
    };

    //! constructor
    CustomDialog(
        QWidget *parent,
        const unsigned int& flags = OkButton | CancelButton,
        Qt::WFlags wflags = 0);

    //! destructor
    virtual ~CustomDialog( void )
    {}

    //! retrieve main vbox
    QBoxLayout& mainLayout( void ) const
    {
        assert( mainLayout_ );
        return *mainLayout_;
    }

    //! retrieve button layout
    QBoxLayout& buttonLayout( void ) const
    {
        assert( buttonLayout_ );
        return *buttonLayout_;
    }

    //! returns true if OK button is valid
    bool hasOkButton( void )
    { return okButton_; }

    //! retrieve OK button
    QPushButton& okButton( void ) const
    {
        assert( okButton_ );
        return *okButton_;
    }

    //! returns true if Cancel button is valid
    bool hasCancelButton( void )
    { return cancelButton_; }

    //! retrieve CANCEL button
    QPushButton& cancelButton( void ) const
    {
        assert( cancelButton_ );
        return *cancelButton_;
    }


    private:

    //! main layout
    QBoxLayout *mainLayout_;

    //! button hbox
    QBoxLayout *buttonLayout_;

    //! ok QPushButton, if any
    QPushButton *okButton_;

    //! cancel QPushButton, if any
    QPushButton *cancelButton_;

};

#endif


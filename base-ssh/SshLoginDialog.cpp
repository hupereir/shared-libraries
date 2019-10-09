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

#include "SshLoginDialog.h"
#include "GridLayout.h"
#include "PasswordEditor.h"
#include "QtUtil.h"

#include <QLabel>

namespace Ssh
{

    //___________________________________________________________________________
    LoginDialog::LoginDialog( QWidget* parent ):
        CustomDialog( parent, OkButton|CancelButton|Separator )
    {

        Debug::Throw( "LoginDialog::LoginDialog.\n" );
        setOptionName( "SSH_LOGIN_DIALOG" );
        setWindowTitle( tr( "SSH login" ) );
        auto gridLayout = new GridLayout;
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        mainLayout().addLayout( gridLayout );

        QLabel* label;

        // user name
        gridLayout->addWidget( label = new QLabel( tr( "User name:" ), this ) );
        gridLayout->addWidget( userNameEditor_ = new LineEditor( this ) );
        label->setBuddy( userNameEditor_ );

        // password
        gridLayout->addWidget( label = new QLabel( tr( "Password:" ), this ) );
        gridLayout->addWidget( passwordEditor_ = new PasswordEditor( this ) );
        label->setBuddy( passwordEditor_ );
        gridLayout->setColumnStretch( 1, 1 );

        // remember password
        gridLayout->addWidget( rememberPaswordCheckBox_ = new QCheckBox( tr( "Remember password" ) ), gridLayout->currentRow(), 1, 1, 1 );
    }

    //______________________________________________________________________
    ConnectionAttributes LoginDialog::attributes() const
    {
        auto attributes( attributes_ );
        attributes.setUserName( userNameEditor_->text() );
        attributes.setPassword( passwordEditor_->text() );
        attributes.setRememberPassword( rememberPaswordCheckBox_->isChecked() );
        return attributes;
    }

    //______________________________________________________________________
    void LoginDialog::setAttributes( const ConnectionAttributes& attributes )
    {
        attributes_ = attributes;
        if( !attributes.host().isEmpty() ) setWindowTitle( tr( "SSH login into %1" ).arg( attributes.host() ) );
        userNameEditor_->setText( attributes.userName() );
        passwordEditor_->setText( attributes.password() );
        rememberPaswordCheckBox_->setChecked( attributes.rememberPassword() );

        if( attributes.userName().isEmpty() ) userNameEditor_->setFocus();
        else passwordEditor_->setFocus();

    }

}

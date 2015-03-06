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
        setWindowTitle( tr( "SSH login - Homechat" ) );
        GridLayout* gridLayout = new GridLayout();
        gridLayout->setMargin(0);
        gridLayout->setMaxCount(2);
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        mainLayout().addLayout( gridLayout );

        QLabel* label;

        // host name
        gridLayout->addWidget( label = new QLabel( tr( "Host name:" ), this ) );
        gridLayout->addWidget( hostEditor_ = new LineEditor( this ) );
        label->setBuddy( hostEditor_ );

        hiddenWidgets_.append( label );
        hiddenWidgets_.append( hostEditor_ );

        // port
        gridLayout->addWidget( label = new QLabel( tr( "Port:" ), this ) );
        gridLayout->addWidget( portSpinBox_ = new QSpinBox( this ) );
        label->setBuddy( portSpinBox_ );

        hiddenWidgets_.append( label );
        hiddenWidgets_.append( portSpinBox_ );

        // user name
        gridLayout->addWidget( label = new QLabel( tr( "User name:" ), this ) );
        gridLayout->addWidget( userNameEditor_ = new LineEditor( this ) );
        label->setBuddy( userNameEditor_ );

        // password
        gridLayout->addWidget( label = new QLabel( tr( "Password:" ), this ) );
        gridLayout->addWidget( passwordEditor_ = new LineEditor( this ) );
        passwordEditor_->setEchoMode( QLineEdit::Password );
        label->setBuddy( passwordEditor_ );
        gridLayout->setColumnStretch( 1, 1 );
        passwordEditor_->setFocus();

        // remember password
        gridLayout->addWidget( checkBox_ = new QCheckBox( tr( "Remember password" ) ), 4, 1, 1, 1 );

        foreach( auto widget, hiddenWidgets_ )
        { widget->hide(); }

    }

    //______________________________________________________________________
    ConnectionAttributes LoginDialog::attributes( void ) const
    {
        ConnectionAttributes attributes( attributes_ );
        attributes.setHost( hostEditor_->text() );
        attributes.setPort( portSpinBox_->value() );
        attributes.setUserName( userNameEditor_->text() );
        attributes.setPassword( passwordEditor_->text() );
        attributes.setRememberPassword( checkBox_->isChecked() );
        return attributes;
    }

    //______________________________________________________________________
    void LoginDialog::setAttributes( const ConnectionAttributes& attributes )
    {
        attributes_ = attributes;
        if( !attributes.host().isEmpty() ) setWindowTitle( QString( tr( "SSH login into %1" ) ).arg( attributes.host() ) );
        hostEditor_->setText( attributes.host() );
        portSpinBox_->setValue( attributes.port() );
        userNameEditor_->setText( attributes.userName() );
        passwordEditor_->setText( attributes.password() );
        checkBox_->setChecked( attributes.rememberPassword() );
    }
}

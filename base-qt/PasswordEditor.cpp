/******************************************************************************
*
* Copyright (C) 2017 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
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

#include "PasswordEditor.h"

#include "BaseIconNames.h"
#include "IconEngine.h"

//____________________________________________________________
PasswordEditor::PasswordEditor( QWidget* parent ):
    LineEditor( parent )
{
    Debug::Throw( "PasswordEditor::PasswordEditor.\n" );
    setEchoMode( QLineEdit::Password );

    showPasswordButton_ = new LineEditorButton;
    showPasswordButton_->setIcon( IconEngine::get( IconNames::ShowPassword ) );
    addRightWidget( showPasswordButton_ );

    connect( showPasswordButton_, SIGNAL(clicked()), SLOT(_toggleShowPassword()) );
}

//____________________________________________________________
void PasswordEditor::showPassword()
{
    Debug::Throw( "PasswordEditor::showPassword.\n" );
    setEchoMode( Normal );
    showPasswordButton_->setIcon( IconEngine::get( IconNames::HidePassword ) );
}

//____________________________________________________________
void PasswordEditor::hidePassword()
{
    Debug::Throw( "PasswordEditor::hidePassword.\n" );
    setEchoMode( Password );
    showPasswordButton_->setIcon( IconEngine::get( IconNames::ShowPassword ) );
}

//____________________________________________________________
void PasswordEditor::_toggleShowPassword()
{
    Debug::Throw( "PasswordEditor::_toggleShowPassword.\n" );
    if( echoMode() == Normal )
    {

        setEchoMode( Password );
        showPasswordButton_->setIcon( IconEngine::get( IconNames::ShowPassword ) );

    } else {

        setEchoMode( Normal );
        showPasswordButton_->setIcon( IconEngine::get( IconNames::HidePassword ) );

    }

}

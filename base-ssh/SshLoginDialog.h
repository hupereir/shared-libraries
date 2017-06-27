#ifndef SshLoginDialog_h
#define SshLoginDialog_h

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

#include "CustomDialog.h"
#include "LineEditor.h"
#include "SshConnectionAttributes.h"

#include <QCheckBox>
#include <QSpinBox>

namespace Ssh
{
    class LoginDialog: public CustomDialog
    {

        //* Qt meta object declaration
        Q_OBJECT

        public:

        //* constructor
        explicit LoginDialog( QWidget* );

        //*@name accessors
        //@{

        //* connection attributes
        ConnectionAttributes attributes() const;

        //@}

        //*@name modifiers
        //@{

        //* connection attributes
        void setAttributes( const ConnectionAttributes& );

        //@}

        protected Q_SLOTS:

        //* show/hide password
        void _toggleShowPassword( bool );

        private:

        //* connection attributes
        ConnectionAttributes attributes_;

        LineEditor* userNameEditor_ = nullptr;
        LineEditor* passwordEditor_ = nullptr;
        QCheckBox* rememberPaswordCheckBox_ = nullptr;
        QCheckBox* showPasswordCheckBox_ = nullptr;

    };

}

#endif

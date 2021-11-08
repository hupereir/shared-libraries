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

#include "Dialog.h"
#include "LineEditor.h"
#include "SshConnectionAttributes.h"
#include "base_ssh_export.h"

#include <QCheckBox>
#include <QSpinBox>

class PasswordEditor;

namespace Ssh
{
    class BASE_SSH_EXPORT LoginDialog: public Dialog
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

        private:

        //* connection attributes
        ConnectionAttributes attributes_;

        LineEditor* userEditor_ = nullptr;
        PasswordEditor* passwordEditor_ = nullptr;
        QCheckBox* rememberPaswordCheckBox_ = nullptr;

    };

}

#endif

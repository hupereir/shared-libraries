#ifndef SshUtil_h
#define SshUtil_h

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

#include "base_ssh_export.h"

namespace Ssh
{
    namespace Util
    {

        //* proper ssh session cleaning
        class BASE_SSH_EXPORT SessionDeleter
        {
            public:
            void operator() (void*) const;
        };

        //* keeps session as blocking as long as object exists
        class BASE_SSH_EXPORT SessionBlocker
        {
            public:

            //* constructor
            explicit SessionBlocker( void* );

            //* destructor
            ~SessionBlocker();

            private:

            void* session_ = nullptr;

        };

        //* proper sftp session cleaning
        class BASE_SSH_EXPORT SftpDeleter
        {
            public:
            void operator() (void*) const;
        };

        //* proper ssh channel cleaning
        class BASE_SSH_EXPORT ChannelDeleter
        {
            public:
            void operator() (void*) const;
        };

        //* proper sftp file handle cleaning
        class BASE_SSH_EXPORT FileHandleDeleter
        {
            public:
            void operator() (void*) const;
        };

    }
}

#endif

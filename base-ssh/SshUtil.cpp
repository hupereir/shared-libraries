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

#include "SshUtil.h"


#if WITH_SSH
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#endif


namespace Ssh
{
    namespace Util
    {

        //______________________________________________________
        void SessionDeleter::operator() (void* ptr) const
        {
            #if WITH_SSH
            auto session( static_cast<ssh_session>(ptr) );
            ssh_disconnect( session );
            ssh_free( session );
            #endif
        }

        //______________________________________________________
        SessionBlocker::SessionBlocker( void* session ):
            session_( session )
        {
            #if WITH_SSH
            ssh_set_blocking( static_cast<ssh_session>(session), true);
            #endif
        }

        //______________________________________________________
        SessionBlocker::~SessionBlocker()
        {
            #if WITH_SSH
            ssh_set_blocking( static_cast<ssh_session>(session_), false);
            #endif
        }

        //______________________________________________________
        void ChannelDeleter::operator() (void* ptr) const
        {
            #if WITH_SSH
            auto channel( static_cast<ssh_channel>(ptr) );
            if( ssh_channel_is_open( channel ) ) ssh_channel_close( channel );
            ssh_channel_free( channel );
            #endif
        }

        //______________________________________________________
        void SftpDeleter::operator() (void* ptr) const
        {
            #if WITH_SSH
            auto sftp = static_cast<sftp_session>( ptr );
            sftp_free( sftp );
            #endif
        }

        //______________________________________________________
        void FileHandleDeleter::operator() (void* ptr) const
        {
            #if WITH_SSH
            auto file = static_cast<sftp_file>( ptr );
            sftp_close( file );
            #endif
        }
    }
}

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

#include "SshSingleton.h"

#include <QTextStream>

#if defined (Q_OS_WIN)
#include <windows.h>
#endif

#if HAVE_SSH
#include <libssh2.h>
#endif

namespace Ssh
{

    //_______________________________________________
    Singleton& Singleton::get( void )
    {
        static Singleton singleton;
        return singleton;
    }

    //_______________________________________________
    Singleton::Singleton( void ):
        Counter( "Ssh::Singleton" )
    {

        #if defined(Q_OS_WIN)
        // some windows specific initialization
        WSADATA wsadata;
        WSAStartup(MAKEWORD(2,0), &wsadata);
        #endif

        #if HAVE_SSH
        initialized_ = !libssh2_init(0);
        #endif
    }

    //_______________________________________________
    Singleton::~Singleton( void )
    {
        #if HAVE_SSH
        if( initialized_ ) libssh2_exit();
        #endif
    }

}

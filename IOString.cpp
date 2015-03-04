
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

#include "IOString.h"

//_________________________________________________________________
IOString::IOString( QIODevice& device )
{

    forever
    {
        int bytes( device.bytesAvailable() );
        if( bytes <= 0 ) break;

        char* data = new char[bytes+1];
        qint64 bytesRead = device.read( data, bytes );
        if( bytesRead <= 0 ) break;
        data[bytesRead] = '\0';
        append( data );
        delete[] data;
    }

}

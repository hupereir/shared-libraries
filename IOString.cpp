// $Id$

/***********************************************************************
*
* Copyright 2013 Hugo PEREIRA DA COSTA <hugo.pereira@free.fr>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***********************************************************************/

/*!
   \file IOString.h
   \brief read bytes from an IODevice and store into a string
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "IOString.h"



//_________________________________________________________________
IOString::IOString( QIODevice& device )
{

  while( device.bytesAvailable() > 0 )
  {
    int bytes( device.bytesAvailable() );
    char* data = new char[bytes+1];
    qint64 bytes_read = device.read( data, bytes );
    if( bytes_read <= 0 ) return;
    data[bytes_read] = '\0';
    append( data );
    delete[] data;
  }

}

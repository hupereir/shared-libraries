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

#include "Debug.h"

//______________________________________
void Debug::Throw( int level, QString str )
{
    // check level
    if( _get().level_ < level ) return;

    // print string to stream
    _get()._stdStream() << str << flush;

    return;

}

//______________________________________
void Debug::Throw( QString str )
{ Throw( 1, str ); }

//______________________________________
QTextStream& Debug::Throw( int level )
{ return ( _get().level_ < level ) ? _get()._nullStream() : _get()._stdStream(); }

//______________________________________
void Debug::setLevel( const int& level )
{ _get().level_ = level; }

//______________________________________
const int& Debug::level( void )
{ return _get().level_; }

//______________________________________
Debug::Debug( void ):
    level_( 0 ),
    nullStream_( &nullDevice_ ),
    stdStream_( stdout )
{}

//______________________________________
Debug::NullIODevice::NullIODevice()
{}

//______________________________________
qint64 Debug::NullIODevice::readData( char*, qint64 )
{ return 0; }

//______________________________________
qint64 Debug::NullIODevice::writeData( const char*, qint64 )
{ return 0; }

//______________________________________
Debug::NullIODevice& Debug::_nullDevice( void )
{ return nullDevice_; }

//______________________________________
QTextStream& Debug::_nullStream( void )
{ return nullStream_; }

//______________________________________
QTextStream& Debug::_stdStream( void )
{ return stdStream_; }

//_______________________________________________
Debug& Debug::_get( void )
{
    static Debug singleton;
    return singleton;
}

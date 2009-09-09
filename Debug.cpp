// $Id$

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
   \file    Debug.cpp
   \brief   Thread-safe option driven debugging class
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Debug.h"

/*!
\class   Debug
\brief   Thread-safe debugging class
*/

//______________________________________
Debug::LockedStream::LockedStream( QTextStream& stream, QMutex& mutex ):
  mutex_lock_( &mutex ),
  stream_( stream )
{}

//______________________________________
Debug::LockedStream::LockedStream( const LockedStream& ref ):
  mutex_lock_( ref.mutex_lock_.mutex() ),
  stream_( ref.stream_ )
{}

//______________________________________
Debug::LockedStream::~LockedStream( void )
{}

//______________________________________
Debug::LockedStream::operator QTextStream& () const
{ return stream_; }

//______________________________________
void Debug::Throw( int level, QString str )
{
  // lock
  QMutexLocker lock( &_get()._mutex() );

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
Debug::LockedStream Debug::Throw( int level )
{
  // return proper stream depending on debugging level
  return ( _get().level_ < level ) ?
    LockedStream( _get()._nullStream(), _get()._mutex() ) :
    LockedStream( _get()._stdStream(), _get()._mutex() );
}

//______________________________________
void Debug::setLevel( const int& level )
{ _get().level_ = level; }

//______________________________________
const int& Debug::level( void )
{ return _get().level_; }

//______________________________________
Debug::Debug( void ):
  level_( 0 ),
  null_stream_( &null_device_ ),
  std_stream_( stdout )
{}

//______________________________________
QMutex& Debug::_mutex( void )
{ return mutex_; }

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
{ return null_device_; }

//______________________________________
QTextStream& Debug::_nullStream( void )
{ return null_stream_; }

//______________________________________
QTextStream& Debug::_stdStream( void )
{ return std_stream_; }

//_______________________________________________
Debug& Debug::_get( void )
{
  static Debug singleton;
  return singleton;
}

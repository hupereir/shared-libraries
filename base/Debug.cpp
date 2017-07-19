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

#include "Debug.h"
#include "NonCopyable.h"
#include "TimeStamp.h"

#include <QFile>
#include <QIODevice>
#include <QMutex>
#include <QMutexLocker>

//__________________________________________________________
class Debug::Private final: private Base::NonCopyable<Debug::Private>
{

    public:

    //* debug level
    int level_ = 0;

    //* custom stream
    Debug::Stream debugStream_;

};

//_________________________________________________________________
Debug::Stream::Stream():
    stream_( &device_ )
{ device_.open( stdout, QIODevice::WriteOnly ); }

//_________________________________________________________________
void Debug::Stream::setFileName( const QString& filename )
{

    if( device_.isOpen() ) device_.close();
    if( filename.isEmpty() ) device_.open( stdout, QIODevice::WriteOnly );
    else {

        device_.setFileName( filename );
        device_.open( QIODevice::WriteOnly );

    }
}

//_________________________________________________________________
int Debug::level()
{ return _get().level_; }

//_________________________________________________________________
void Debug::setLevel( int level )
{ _get().level_ = level; }

//_________________________________________________________________
void Debug::setFileName( const QString& filename )
{ _get().debugStream_.setFileName( filename ); }

//_________________________________________________________________
void Debug::Throw( int level, const QString& str )
{
    if( _get().level_ >= level )
    {
        _get().debugStream_.get()
            << TimeStamp::now().toString( "yyyy/MM/dd HH:mm:ss" ) << " "
            << str
            << flush;
    }

}

//______________________________________
Debug::Stream& Debug::Throw( int level )
{
    if( _get().level_ >= level )
    {

        // enable
        _get().debugStream_.setEnabled( true );

        // add timestamp
        _get().debugStream_.get() << TimeStamp::now().toString( "yyyy/MM/dd HH:mm:ss" ) << " ";

    } else _get().debugStream_.setEnabled( false );

    // return stream
    return _get().debugStream_;
}

//_______________________________________________
Debug::Private& Debug::_get()
{
    static Debug::Private singleton;
    return singleton;
}

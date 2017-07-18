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

//* null device.
/*! Used to throw everything if the level is not high enough */
class NullIODevice : public QIODevice
{
    public:

    //* constructor
    explicit NullIODevice()
    { setOpenMode( WriteOnly ); }

    protected:

    // read
    virtual qint64 readData ( char*, qint64 )
    { return 0; }

    // read
    virtual qint64 writeData( const char*, qint64 )
    { return 0; }

};

// debug private class
class Debug::Private final: private Base::NonCopyable<Debug::Private>
{

    public:

    //* constructor
    Private():
        nullStream_( &nullDevice_ ),
        stdStream_( &stdDevice_ )
    { stdDevice_.open( stdout, QIODevice::WriteOnly ); }

    //* debug level
    int level_ = 0;

    //* null device
    NullIODevice nullDevice_;

    //* null stream
    QTextStream nullStream_;

    //* file device
    QFile stdDevice_;

    //* default stream
    QTextStream stdStream_;

};

//______________________________________
int Debug::level()
{ return _get().level_; }

//______________________________________
void Debug::setLevel( int level )
{ _get().level_ = level; }

//______________________________________
void Debug::setFileName( QString filename )
{

    if( _get().stdDevice_.isOpen() ) _get().stdDevice_.close();
    if( filename.isEmpty() ) _get().stdDevice_.open( stdout, QIODevice::WriteOnly );
    else {

        _get().stdDevice_.setFileName( filename );
        _get().stdDevice_.open( QIODevice::WriteOnly );

    }
}

//______________________________________
void Debug::Throw( int level, QString str )
{ if( _get().level_ >= level ) _get().stdStream_ << TimeStamp::now().toString( "yyyy/MM/dd HH:mm:ss" ) << " " << str << flush; }

//______________________________________
void Debug::Throw( QString str )
{ Throw( 1, str ); }

//______________________________________
QTextStream& Debug::Throw( int level )
{
    if( _get().level_ < level ) return _get().nullStream_;
    else return _get().stdStream_  << TimeStamp::now().toString( "yyyy/MM/dd HH:mm:ss" ) << " ";
}

//_______________________________________________
Debug::Private& Debug::_get()
{
    static Debug::Private singleton;
    return singleton;
}

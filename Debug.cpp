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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"

//! null device.
/*! Used to throw everything if the level is not high enough */
class NullIODevice : public QIODevice
{
    public:

    //! constructor
    NullIODevice()
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
class Debug::Private
{

    public:

    //! constructor
    Private( void ):
        level_( 0 ),
        nullStream_( &nullDevice_ ),
        stdStream_( &stdDevice_ )
    {

        // by default open stdout
        stdDevice_.open( stdout, QIODevice::WriteOnly );

    }

    //! debug level
    int level_;

    //! null device
    NullIODevice nullDevice_;

    //! null stream
    QTextStream nullStream_;

    //! file device
    QFile stdDevice_;

    //! default stream
    QTextStream stdStream_;

};

//______________________________________
const int& Debug::level( void )
{ return _get().level_; }

//______________________________________
void Debug::setLevel( const int& level )
{ _get().level_ = level; }

//______________________________________
void Debug::setFileName( const QString& filename )
{
    if( filename.isEmpty() )
    {

        if( _get().stdDevice_.isOpen() ) _get().stdDevice_.close();
        _get().stdDevice_.open( stdout, QIODevice::WriteOnly );

    } else {

        // close file device
        if( _get().stdDevice_.isOpen() ) _get().stdDevice_.close();
        _get().stdDevice_.setFileName( filename );
        _get().stdDevice_.open( QIODevice::WriteOnly );


    }

}

//______________________________________
void Debug::Throw( int level, QString str )
{
    // check level
    if( _get().level_ < level ) return;

    // print string to stream
    _get().stdStream_ << str << flush;

    return;

}

//______________________________________
void Debug::Throw( QString str )
{ Throw( 1, str ); }

//______________________________________
QTextStream& Debug::Throw( int level )
{ return ( _get().level_ < level ) ? _get().nullStream_ : _get().stdStream_; }

//_______________________________________________
Debug::Private& Debug::_get( void )
{
    static Debug::Private singleton;
    return singleton;
}

#ifndef Command_h
#define Command_h

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

#include "Counter.h"

#include <QString>
#include <QStringList>

class Command: public QStringList, public Counter
{

    public:

    //! constructor
    Command():
        Counter( "Command" )
     {}

    //! constructor
    Command( const QStringList& other ):
        QStringList( other ),
        Counter( "Command" )
    {}

    //! constructor
    Command( const QString& in ):
        QStringList( _parse( in ) ),
        Counter( "Command" )
    {}

    // run
    bool run( const QString& = QString() ) const;

    // streamers
    Command & operator<< ( const QString & str )
    {
        QStringList::operator << ( str );
        return *this;
    }

    // streamers
    Command & operator<< ( const QStringList & other )
    {
        QStringList::operator << ( other );
        return *this;
    }

    private:

    //! parse command
    /*!
    parse command so that first string in the list
    is the command name and following strings are all arguments
    first argument must start with a "-"
    */
    QStringList _parse( const QString& ) const;

};

#endif

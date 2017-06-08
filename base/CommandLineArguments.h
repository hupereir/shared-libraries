#ifndef CommandLineArguments_h
#define CommandLineArguments_h

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
#include "Debug.h"

#include <QStringList>

class CommandLineArguments: public QStringList, private Base::Counter<CommandLineArguments>
{

    public:

    //* constructor
    explicit CommandLineArguments( int argc = 0, char* argv[] = nullptr );

    //* constructor
    explicit CommandLineArguments( const QStringList& );

    //* return arg counts
    int argc( void ) const
    { return size(); }

    //* return argv
    char** argv( void ) const;

};

#endif

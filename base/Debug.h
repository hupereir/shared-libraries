#ifndef Debug_h
#define Debug_h

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

#include <QString>
#include <QTextStream>

class Debug
{
    public:

    //*@name static accessors
    //@{

    //* retrieves the debug level
    static int level();

    //@}

    //*@name static modifiers
    //@{

    //* sets the debug level. Everything thrown of bigger level is not discarded
    static void setLevel( int );

    //* set file name
    static void setFileName( QString );

    //* writes string to clog if level is lower than level_
    static void Throw( int, QString );

    //* writes string to clog if level_ is bigger than 0
    static void Throw( QString );

    //* returns either clog or dummy stream depending of the level
    static QTextStream& Throw( int = 1 );

    //@}

    private:

    class Private;

    //* return singleton
    static Private& _get();

};

#endif

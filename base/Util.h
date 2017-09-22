#ifndef Util_h
#define Util_h

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

#include "File.h"

#include <QStringList>

//* some utilities used everywhere
class Util
{
    public:

    //* square a number
    template< typename T >
    static T square( const T& t)
    { return t*t; }

    //* returns environment variable or empty string
    static QString env( const QString&, const QString& = QString() );

    //*  user name
    static QString user();

    //* returns the domain name
    static QString domain();

    //* user home directory
    static File home();

    //* temporary directory ("/tmp")
    static File tmp();

    //* user config path
    static File config();

    //* user data path
    static File data();

    //* user cache path
    static File cache();

    /** returns the host name.
    \par short_name if true returns only the string located before the first '.'
    */
    static QString host( bool shortName = false );

    //* window title separator
    static QString windowTitleSeparator();

    //* append host to input string to form window title
    static QString windowTitle( const QString& );

    //* application pid
    static int pid();

    //* returns the work directory
    static File workingDirectory();

};

#endif

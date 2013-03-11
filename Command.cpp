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

#include "Command.h"
#include "Debug.h"

#include <QProcess>

//____________________________________________
bool Command::run( const QString& path ) const
{

    Debug::Throw( "Command::run.\n" );
    if( empty() ) return false;

    QString program( front() );
    QStringList arguments( *this );
    arguments.removeFirst();
    return QProcess::startDetached( program, arguments, path );

}

//_________________________________________________________
QStringList Command::_parse( const QString &in ) const
{

    Debug::Throw( "Command::parse.\n" );
    QStringList out;

    // first split using '"' as separator to get "single string" arguments that contain strings
    QStringList local = in.split( "\"", QString::KeepEmptyParts );

    // split strings that are not enclosed into quotes using white spaces as separator
    bool split( true );
    for( QStringList::const_iterator iter = local.begin(); iter != local.end(); ++iter )
    {
        if( !iter->isEmpty() )
        {
            if( split ) out << iter->split( QRegExp( "\\s+" ), QString::SkipEmptyParts );
            else out << QString("\"") + *iter + "\"" ;
        }

        split = !split;
    }

    // print output
    if( Debug::level() >= 1 )
    {
        for( QStringList::const_iterator iter = out.begin(); iter != out.end(); ++iter )
        { Debug::Throw(0) << "Command::parse - \"" << *iter << "\"" << endl; }
    }

    return out;

}

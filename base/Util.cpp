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

#include "Util.h"
#include "Debug.h"
#include "File.h"

#include <QDir>
#include <QProcess>
#include <QHostInfo>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

#include <unistd.h>

//______________________________________________________________________
QString Util::env( const QString& val, const QString& defaultValue )
{

    for( const auto& entry:QProcess::systemEnvironment() )
    {

        // TODO: should use regular expression
        int position( entry.indexOf( "=" ) );
        if( position <= 0 ) continue;

        auto var( entry.left( position ) );
        if( var == QString( val ) ) return entry.mid( position+1 );
    }

    return defaultValue;

}

//______________________________________________________________________
QString Util::user()
{
    Debug::Throw( "Util::user.\n" );
    #if defined(Q_OS_WIN)
    return env( "USERNAME", QObject::tr( "unknown user" ) );
    #else
    // first try look for USERNAME environment variable
    // if failed, try USER
    // if failed, return unknown.
    return env( "USERNAME", env( "USER", QObject::tr( "unknown user" ) ) );
    #endif
}

//______________________________________________________________________
QString Util::domain()
{
    Debug::Throw( "Util::domain.\n" );

    #if QT_VERSION >= 0x040500

    return QHostInfo::localDomainName();

    #else

    // use system environment.
    // should work for windows
    return env( "USERDOMAIN", "localdomain" );
    #endif
}

//_____________________________________________________
QString Util::windowTitle( const QString& title )
{

    Debug::Throw( "Util::windowTitle.\n" );

    QString host( Util::host() );
    if( host.contains( "localhost" ) ) return title;
    else return QString( "%1 [%2]" ).arg( title, host );

    return title;

}

//______________________________________________________________________
File Util::home()
{ return File( QDir::homePath() ); }

//______________________________________________________________________
File Util::tmp()
{ return File( QDir::tempPath() ); }

//______________________________________________________________________
File Util::config()
{

    #if QT_VERSION >= 0x050000
    return File( QStandardPaths::writableLocation( QStandardPaths::ConfigLocation ) );
    #else
    return File( ".config/" ).addPath( home() );
    #endif

}

//______________________________________________________________________
QString Util::host( bool shortName )
{

    // use system environment
    // it does not work for windows
    Debug::Throw( "Util::host.\n" );
    auto out( QHostInfo::localHostName() );
    if( ! shortName ) return out;
    else {

        int pos( out.indexOf( "." ) );
        return ( pos <0 ) ? out:out.left( pos );

    }

}

//______________________________________________________________________
int Util::pid()
{ return getpid(); }

//______________________________________________________________________
File Util::workingDirectory()
{ return File( QDir::currentPath() ); }

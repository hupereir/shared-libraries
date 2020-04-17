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
#include <QHostInfo>

#include <QProcessEnvironment>
#include <QCoreApplication>

#include <QStandardPaths>

#include <algorithm>
#include <unistd.h>

//______________________________________________________________________
QString Util::env( const QString& key, const QString& defaultValue )
{ return QProcessEnvironment::systemEnvironment().value( key, defaultValue ); }

//______________________________________________________________________
QString Util::user()
{
    Debug::Throw( QStringLiteral("Util::user.\n") );
    #if defined(Q_OS_WIN)
    return env( "USERNAME", QObject::tr( "unknown user" ) );
    #else
    // first try look for USERNAME environment variable
    // if failed, try USER
    // if failed, return unknown.
    return env( QStringLiteral("USERNAME"), env( QStringLiteral("USER"), QObject::tr( "unknown user" ) ) );
    #endif
}

//______________________________________________________________________
QString Util::domain()
{ return QHostInfo::localDomainName(); }

//_____________________________________________________
QString Util::windowTitleSeparator()
{ return QStringLiteral( " \u2014 " ); }

//_____________________________________________________
QString Util::windowTitle( const QString& title )
{

    Debug::Throw( QStringLiteral("Util::windowTitle.\n") );
    const auto applicationName( qApp->applicationName() );
    return title.endsWith( applicationName ) ? title:
        title + windowTitleSeparator() + applicationName;
}

//______________________________________________________________________
File Util::home()
{ return File( QDir::homePath() ); }

//______________________________________________________________________
File Util::tmp()
{ return File( QDir::tempPath() ); }

//______________________________________________________________________
File Util::config()
{ return File( QStandardPaths::writableLocation( QStandardPaths::ConfigLocation ) ); }

//______________________________________________________________________
File Util::data()
{ return File( QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) ); }

//______________________________________________________________________
File Util::cache()
{ return File( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation ) ); }

//______________________________________________________________________
QString Util::host( bool shortName )
{

    // use system environment
    // it does not work for windows
    Debug::Throw( QStringLiteral("Util::host.\n") );
    auto out( QHostInfo::localHostName() );
    if( ! shortName ) return out;
    else {

        int pos( out.indexOf( QLatin1String(".") ) );
        return ( pos <0 ) ? out:out.left( pos );

    }

}

//______________________________________________________________________
int Util::pid()
{ return getpid(); }

//______________________________________________________________________
File Util::workingDirectory()
{ return File( QDir::currentPath() ); }

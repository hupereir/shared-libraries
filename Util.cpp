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

#include "Util.h"
#include "Str.h"
#include "Debug.h"

#include <QDir>
#include <QProcess>
#include <QStringList>
#include <QHostInfo>

#include <unistd.h>

static const int LONGSTR = 256;

//______________________________________________________________________
QString Util::env( const QString& val, const QString& defaultValue )
{

    foreach( const QString& entry, QProcess::systemEnvironment() )
    {

        // TODO: should use regular expression
        int position( entry.indexOf( "=" ) );
        if( position <= 0 ) continue;

        QString var( entry.left( position ) );
        if( var == QString( val ) ) return entry.mid( position+1 );
    }

    return defaultValue;

}

//______________________________________________________________________
QString Util::user( void )
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
QString Util::domain( void )
{
    Debug::Throw( "Util::domain.\n" );

    #if QT_VERSION >= 0x040500

    return QHostInfo::localDomainName();

    #else
    #if defined(Q_WS_X11)

    // use build-in unix function
    char *buf = new char[ LONGSTR ];
    if( !buf ) return "";
    getdomainname( buf, LONGSTR );
    QString out( buf );
    delete[] buf;
    return out;

    #else

    // use system environment.
    // should work for windows
    return env( "USERDOMAIN", "localdomain" );
    #endif
    #endif
}

//_____________________________________________________
QString Util::windowTitle( const QString& title )
{

    Debug::Throw( "Util::windowTitle.\n" );

    #if defined(Q_WS_X11)
    QString host( Util::host() );
    if( host == "localhost" ) return title;
    else return QString( "%1 [%2]" ).arg( title ).arg( host );
    #endif

    return title;

}

//______________________________________________________________________
QString Util::home( void )
{ return QDir::homePath(); }

//______________________________________________________________________
QString Util::tmp( void )
{ return QDir::tempPath(); }

//______________________________________________________________________
QString Util::host( bool shortName )
{

    // use system environment
    // it does not work for windows
    Debug::Throw( "Util::host.\n" );
    QString out( QHostInfo::localHostName() );
    if( ! shortName ) return out;
    else {

        int pos( out.indexOf( "." ) );
        if( pos <0 ) return out;
        return out.left( pos );

    }

}

//______________________________________________________________________
int Util::pid( void )
{ return getpid(); }

//______________________________________________________________________
QString Util::workingDirectory( void )
{ return QDir::currentPath(); }

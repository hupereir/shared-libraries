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


/*!
\file    Util.cpp
\brief   some facilities used everywhere
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include "CustomProcess.h"
#include "Util.h"
#include "Str.h"
#include "Debug.h"

#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#include <QtNetwork/QHostInfo>

#include <unistd.h>

/*! \brief
static 'long' QString size
for c_like system QString access
*/

static const int LONGSTR = 256;

//______________________________________________________________________
QString Util::env( const QString& val, const QString& default_value )
{

    QStringList environment( QProcess::systemEnvironment() );
    for( QStringList::iterator iter = environment.begin(); iter != environment.end(); ++iter )
    {
        int position( iter->indexOf( "=" ) );
        if( position <= 0 ) continue;

        QString var( iter->left( position ) );
        if( var == QString( val ) ) return iter->mid( position+1 );
    }

    return default_value;

}

//______________________________________________________________________
QString Util::user( void )
{
    Debug::Throw( "Util::user.\n" );
    #if defined(Q_WS_WIN)
    return env( "USERNAME", "unknown user" );
    #else
    // first try look for USERNAME environment variable
    // if failed, try USER
    // if failed, return unknown.
    return env( "USERNAME", env( "USER", "unknown user" ) );
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
    return env( "USERDOMAIN","localdomain");
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
    else {
        QString buffer;
        QTextStream( &buffer ) << title << " [" << Util::host() << "]";
        return buffer;
    }
    #endif

    return title;

}

//______________________________________________________________________
QString Util::home( void )
{
    Debug::Throw( "Util::home.\n" );
    return QDir::homePath();
}

//______________________________________________________________________
QString Util::tmp( void )
{
    Debug::Throw( "Util::tmp.\n" );
    return QDir::tempPath();
}
//______________________________________________________________________
QString Util::host( bool short_name )
{

    // use system environment
    // it does not work for windows
    Debug::Throw( "Util::host.\n" );
    QString out( QHostInfo::localHostName() );
    if( ! short_name ) return out;
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

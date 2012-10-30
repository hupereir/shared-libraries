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

#include "SvgPlasmaInterface.h"
#include "Util.h"

#include <QtCore/QStringList>

namespace SVG
{

    //_________________________________________________
    SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
        QObject( parent ),
        valid_( false ),
        imagePath_( "widgets/background" ),
        fileSystemWatcher_(0)
    {}

    //_________________________________________________
    SvgPlasmaInterface::~SvgPlasmaInterface( void )
    {}

    //_________________________________________________
    bool SvgPlasmaInterface::loadTheme( void )
    {

        QStringList configurationFiles;
        configurationFiles
            << ".kde/share/config/plasmarc"
            << ".kde4/share/config/plasmarc";

        // open file
        foreach( const QString& filename, configurationFiles )
        {

            File file = File( filename ).addPath( Util::home() );
            if( !file.exists() ) continue;

            configurationFile_ = file;
            QFile in( configurationFile_ );
            if ( !in.open( QIODevice::ReadOnly ) ) continue;

            // add file to file system watcher
            if( !_hasFileSystemWatcher() )
            {
                _initializeFileSystemWatcher();
                _fileSystemWatcher().addPath( configurationFile_ );
            }

            // get all lines inside QStringList
            QStringList lines = QString( in.readAll() ).split( '\n' );

            bool found_theme( false );
            QRegExp themeTag( "^\\s*\\[Theme\\]\\s*$" );
            QRegExp sectionTag( "^\\[\\S*\\]\\s*$" );
            QRegExp emptyList( "^\\s*$" );
            QRegExp nameTag( "^\\s*name\\s*=\\s*(\\S*)\\s*$" );

            QString theme;
            for( QStringList::const_iterator iter = lines.begin(); iter != lines.end(); ++iter )
            {
                if( !found_theme )
                {

                    if( iter->indexOf( themeTag ) >= 0 ) found_theme = true;
                    continue;

                } else if( iter->indexOf( sectionTag ) >= 0 ) break;
                else if( iter->indexOf( emptyList ) >= 0 ) continue;
                else if( iter->indexOf( nameTag ) >= 0 ) {

                    theme = nameTag.cap(1);
                    break;

                } else continue;

            }

            if( theme.isNull() || theme.isEmpty() ) theme = "default";
            return _setTheme( theme );

        }

        // configuration file not found.
        bool changed( false );
        changed |= _setValid( false );
        changed |= _setPath( QString() );
        return changed;

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadFile( void )
    {

        // check path
        if( !_path().exists() ) return _setValid( false );

        // generate possible file names
        QStringList files;
        files
            << _imagePath()
            << _imagePath() + ".svg"
            << _imagePath() + ".svgz";

        bool found( false );
        File filename;
        foreach( const QString& imageFilename, files )
        {

            filename = File( imageFilename ).addPath( _path() );
            if( filename.exists() )
            {
                found = true;
                break;
            }

        }

        // construct full filename base on theme and transparency setting
        bool changed( false );
        changed |= _setValid( found );
        changed |= _setFileName( found ? filename:File() );
        return changed;

    }

    //_________________________________________________
    void SvgPlasmaInterface::_configurationFileChanged( const QString& file )
    {
        if( loadTheme() ) {
            loadFile();
            emit themeChanged();
        }
    }

    //_____________________________________________________________
    void SvgPlasmaInterface::_initializeFileSystemWatcher( void )
    {
        fileSystemWatcher_ = new QFileSystemWatcher();
        connect( &_fileSystemWatcher(), SIGNAL( fileChanged( const QString& ) ), SLOT( _configurationFileChanged( const QString& ) ) );
    }

    //_________________________________________________
    bool SvgPlasmaInterface::_setTheme( const QString& theme )
    {

        // try use user-scoped path
        QStringList localPath;
        localPath
            << ".kde/share/apps/desktoptheme/"
            << ".kde4/share/apps/desktoptheme/";

        if( theme != "default" )
        {
            foreach( const QString& pathName, localPath )
            {

                File path = File( theme ).addPath( File( pathName ).addPath( Util::home() ) );
                if( path.exists() ) return _setPath( path );
            }
        }

        // try use global path
        File themePath = File( theme ).addPath( File( "/usr/share/apps/desktoptheme/" ) );
        if( themePath.exists() ) return _setPath( themePath );

        // try use default theme
        if( theme == "default" )
        {
            bool changed( false );
            changed |= _setPath( QString() );
            changed |= _setValid( false );
            return changed;
        } else return _setTheme( "default" );

    }

};

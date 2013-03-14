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

#include "Command.h"
#include "CustomProcess.h"
#include "Debug.h"
#include "SvgSystemOptions.h"
#include "Util.h"

#include <QStringList>
#include <QSettings>

namespace SVG
{

    //_________________________________________________
    SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
        QObject( parent ),
        Counter( "SVG::SvgPlasmaInterface" ),
        valid_( false ),
        imagePath_( "widgets/background" ),
        fileSystemWatcher_(0)
    { installSvgOptions(); }

    //_________________________________________________
    bool SvgPlasmaInterface::setImagePath( ImagePath id )
    {
        QString value;
        switch( id )
        {
            case DialogBackground:
            value = "dialogs/background";
            break;

            default:
            case WidgetBackground:
            value = "widgets/background";
            break;

            case WidgetTranslucentBackground:
            value = "widgets/translucentbackground";
            break;

        }

        if( imagePath_ == value ) return false;
        imagePath_ = value;
        return true;

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadTheme( void )
    {

        File::List configurationFiles;

        // TODO: use kde4-config to get the path of all config files
        if( XmlOptions::get().contains( "KDE4_CONFIG" ) )
        {

            // get kde4 config command and retrieve output
            const QString kde4ConfigCommand( XmlOptions::get().raw( "KDE4_CONFIG" ) );

            CustomProcess process( this );
            process.start( Command( kde4ConfigCommand ) << "--path" << "config" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                const QStringList configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( QRegExp(":") );
                foreach( const QString& path, configurationPath )
                { configurationFiles << File( "plasmarc" ).addPath( path ); }

            }

        }

        if( configurationFiles.isEmpty() )
        {
            configurationFiles
                << File( ".kde/share/config/plasmarc" ).addPath( Util::home() )
                << File( ".kde4/share/config/plasmarc" ).addPath( Util::home() );
        }

        QString theme;
        foreach( const File& file, configurationFiles )
        {
            if( !file.exists() ) continue;

            Debug::Throw(0) << "SvgPlasmaInterface::loadTheme - checking: " << file << endl;

            configurationFile_ = file;

            // read group
            QSettings settings( configurationFile_, QSettings::IniFormat );
            settings.sync();
            Debug::Throw(0) << "SvgPlasmaInterface::loadTheme - keys: " << settings.allKeys().join( "\n" ) << endl;
            if( settings.contains( "Theme/name" ) )
            {
                theme = settings.value( "Theme/name" ).toString();
                Debug::Throw(0) << "SvgPlasmaInterface::loadTheme - theme: " << theme << endl;
                break;
            }

        }

        // try use default theme if none found
        if( theme.isEmpty() ) theme = "default";
        if( !_setTheme( theme ) )
        {

            // configuration file not found.
            bool changed( false );
            changed |= _setValid( false );
            changed |= _setPath( QString() );
            return changed;

        } else return true;

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadFile( void )
    {

        // check path
        if( !path_.exists() ) return _setValid( false );

        // generate possible file names
        QStringList files;
        files
            << imagePath_
            << imagePath_ + ".svg"
            << imagePath_ + ".svgz";

        bool found( false );
        File filename;
        foreach( const QString& imageFilename, files )
        {

            filename = File( imageFilename ).addPath( path_ );
            if( filename.exists() )
            {
                found = true;
                break;
            }

        }

        Debug::Throw() << "SvgPlasmaInterface::loadFile - filename: " << filename << endl;

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

        if( theme != "default" )
        {
            // try use local path
            QStringList localPath;
            localPath
                << ".kde/share/apps/desktoptheme/"
                << ".kde4/share/apps/desktoptheme/";

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

}

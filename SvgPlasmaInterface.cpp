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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
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
        valid_( false ),
        imagePath_( WidgetBackground ),
        fileSystemWatcher_(0)
    { installSvgSystemOptions(); }

    //_________________________________________________
    bool SvgPlasmaInterface::setImagePath( ImagePath id )
    {
        if( imagePath_ == id ) return false;
        imagePath_ = id;
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
            // add some files manually in case the above failed
            configurationFiles
                << File( ".kde/share/config/plasmarc" ).addPath( Util::home() )
                << File( ".kde4/share/config/plasmarc" ).addPath( Util::home() );
        }

        // make sure fileSystemWatcher is valid
        if( !fileSystemWatcher_ )
        {
            fileSystemWatcher_ = new QFileSystemWatcher( this );
            connect( fileSystemWatcher_, SIGNAL( fileChanged( const QString& ) ), SLOT( _configurationFileChanged( const QString& ) ) );
        }

        // add valid configuration files to watcher
        QStringList oldFiles( fileSystemWatcher_->files() );
        foreach( const File& file, configurationFiles )
        { if( file.exists() && !oldFiles.contains( file ) ) fileSystemWatcher_->addPath( file ); }

        // look for theme in selected configuration files
        QString theme( "default" );
        foreach( const File& file, configurationFiles )
        {
            if( !file.exists() ) continue;

            Debug::Throw() << "SvgPlasmaInterface::loadTheme - checking: " << file << endl;

            // read group
            QSettings settings( file, QSettings::IniFormat );
            settings.sync();
            if( settings.contains( "Theme/name" ) )
            {
                theme = settings.value( "Theme/name" ).toString();
                break;
            }

        }

        return _setTheme( theme );

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadFile( void )
    {

        // check path
        if( !path_.exists() ) return _setValid( false );

        File filename;
        const bool found =
            !( filename = _findImage( path_, imagePath_ ) ).isEmpty() ||
            !( filename = _findImage( path_, WidgetBackground ) ).isEmpty();

        // construct full filename base on theme and transparency setting
        bool changed( false );
        changed |= _setValid( found );
        changed |= _setFileName( found ? filename:File() );
        return changed;

    }

    //_________________________________________________
    void SvgPlasmaInterface::_configurationFileChanged( const QString& file )
    {

        Debug::Throw() << "SvgPlasmaInterface::_configurationFileChanged - file: " << file << endl;
        if( loadTheme() )
        {
            loadFile();
            emit themeChanged();
        }
    }

    //_________________________________________________
    bool SvgPlasmaInterface::_setTheme( const QString& theme )
    {

        Debug::Throw() << "SvgPlasmaInterface::_setTheme - theme:" << theme << endl;
        File::List themePathList;

        if( theme != "default" )
        {
            // add local path
            QStringList localPath;
            localPath
                << ".kde/share/apps/desktoptheme/"
                << ".kde4/share/apps/desktoptheme/";

            foreach( const QString& pathName, localPath )
            { themePathList << File( theme ).addPath( File( pathName ).addPath( Util::home() ) ); }

        }

        // add local path
        themePathList << File( theme ).addPath( File( "/usr/share/apps/desktoptheme/" ) );
        foreach( const File& themePath, themePathList )
        {
            if( themePath.exists() && !_findImage( themePath, WidgetBackground ).isEmpty() )
            { return _setPath( themePath ); }
        }

        // try use default theme
        if( theme == "default" )
        {

            bool changed( false );
            changed |= _setPath( QString() );
            changed |= _setValid( false );
            return changed;

        } else return _setTheme( "default" );

    }

    //_________________________________________________
    File SvgPlasmaInterface::_imagePath( SvgPlasmaInterface::ImagePath id ) const
    {
        switch( id )
        {
            default:
            case WidgetBackground: return File( "widgets/background" );
            case DialogBackground: return File( "dialogs/background" );
            case WidgetTranslucentBackground: return  File( "widgets/translucentbackground" );
        }

    }

    //_________________________________________________
    File SvgPlasmaInterface::_findImage( const File& path, SvgPlasmaInterface::ImagePath id ) const
    {

        // check path existence
        if( !path.exists() ) return File();

        // build list of candidates
        const File imagePath( _imagePath( id ) );
        const File::List files = File::List()
            << imagePath
            << File( imagePath + ".svg" )
            << File( imagePath + ".svgz" );
        foreach( File file, files )
        { if( (file = file.addPath( path ) ).exists() ) return file; }

        return File();
    }

}

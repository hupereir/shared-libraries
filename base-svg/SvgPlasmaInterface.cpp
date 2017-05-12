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

namespace Svg
{

    //_________________________________________________
    SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
        QObject( parent )
    { installSvgSystemOptions(); }

    //_________________________________________________
    QPalette SvgPlasmaInterface::themePalette( void ) const
    {

        QPalette out;

        QSettings settings( themePaletteFilename_, QSettings::IniFormat );
        settings.sync();

        auto updateColor = [&out,&settings](QPalette::ColorRole role, QString name)
        {
            if( settings.contains( name ) )
            {
                auto rgb( settings.value( name ).toStringList() );
                if( rgb.size() == 3 )
                { out.setColor( role, QColor( rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt() ) ); }
            }
        };

        updateColor( QPalette::WindowText, "Colors:Window/ForegroundNormal" );
        updateColor( QPalette::Window, "Colors:Window/BackgroundNormal" );
        updateColor( QPalette::Highlight, "Colors:Selection/BackgroundNormal" );
        updateColor( QPalette::Text, "Colors:View/ForegroundNormal" );
        updateColor( QPalette::Base, "Colors:View/BackgroundNormal" );
        updateColor( QPalette::ButtonText, "Colors:Button/ForegroundNormal" );
        updateColor( QPalette::Button, "Colors:Button/BackgroundNormal" );
        return out;

    }

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

        Debug::Throw(0) << "Svg::SvgPlasmaInterface::loadTheme" << endl;

        File::List configurationFiles;
        if( XmlOptions::get().contains( "KDE_CONFIG" ) )
        {

            // get kde4 config command and retrieve output
            const QString kdeConfigCommand( XmlOptions::get().raw( "KDE_CONFIG" ) );

            CustomProcess process( this );
            process.start( Command( kdeConfigCommand ) << "--path" << "config" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                auto configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( ':' );
                for( const auto& path:configurationPath )
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
            connect( fileSystemWatcher_, SIGNAL(fileChanged(QString)), SLOT(_configurationFileChanged(QString)) );
        }

        // add valid configuration files to watcher
        auto oldFiles( fileSystemWatcher_->files() );
        for( const auto& file:configurationFiles )
        { if( file.exists() && !oldFiles.contains( file ) ) fileSystemWatcher_->addPath( file ); }

        // look for theme in selected configuration files
        QString theme( "default" );
        for( const auto& file:configurationFiles )
        {
            if( !file.exists() ) continue;

            Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - checking: " << file << endl;

            // read group
            QSettings settings( file, QSettings::IniFormat );
            settings.sync();
            if( settings.contains( "Theme/name" ) )
            {
                theme = settings.value( "Theme/name" ).toString();
                break;
            }

        }

        Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - using theme: " << theme << endl;

        // reset colorFilename
        auto oldThemePaletteFilename = themePaletteFilename_;
        themePaletteFilename_.clear();

        // assign theme
        bool modified = _setTheme( theme );

        // return modified state
        return modified || (themePaletteFilename_ != oldThemePaletteFilename);

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadFile( void )
    {

        Debug::Throw(0) << "Svg::SvgPlasmaInterface::loadFile" << endl;

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

        Debug::Throw(0) << "Svg::SvgPlasmaInterface::_configurationFileChanged - file: " << file << endl;
        if( loadTheme() )
        {
            loadFile();
            emit themeChanged();
        }
    }

    //_________________________________________________
    bool SvgPlasmaInterface::_setTheme( const QString& theme )
    {

        Debug::Throw() << "Svg::SvgPlasmaInterface::_setTheme - theme:" << theme << endl;
        File::List themePathList;

        if( XmlOptions::get().contains( "KDE_CONFIG" ) )
        {

            // get kde4 config command and retrieve output
            const QString kdeConfigCommand( XmlOptions::get().raw( "KDE_CONFIG" ) );

            CustomProcess process( this );
            process.start( Command( kdeConfigCommand ) << "--path" << "data" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                const QStringList configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( QRegExp(":") );

                #if QT_VERSION >= 0x050000
                for( const auto& path:configurationPath )
                { themePathList << File( theme ).addPath( File( "plasma/desktoptheme/" ).addPath( path ) ); }
                #else
                for( const auto& path:configurationPath )
                { themePathList << File( theme ).addPath( File( "desktoptheme/" ).addPath( path ) ); }
                #endif

            }

        }

        if( themePathList.empty() )
        {
            // add local path
            QStringList localPath = { ".kde/share/apps/desktoptheme/", ".kde4/share/apps/desktoptheme/" };
            for( const auto& pathName:localPath )
            { themePathList << File( theme ).addPath( File( pathName ).addPath( Util::home() ) ); }

        }

        // add local path
        themePathList << File( theme ).addPath( File( "/usr/share/apps/desktoptheme/" ) );
        for( const auto& themePath:themePathList )
        {
            Debug::Throw() << "Svg::SvgPlasmaInterface::_setTheme - checking " << themePath << endl;
            if( themePath.exists() )
            {

                // try load color palette
                if( themePaletteFilename_.isEmpty() )
                {
                    File file = File( "colors" ).addPath( themePath );
                    if( file.exists() ) themePaletteFilename_ = file;
                }

                if( !_findImage( themePath, WidgetBackground ).isEmpty() )
                { return _setPath( themePath ); }

            }

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
    bool SvgPlasmaInterface::_setPath( const File& path )
    {
        if( path_ == path ) return false;
        path_ = path;
        return true;
    }

    //_________________________________________________
    bool SvgPlasmaInterface::_setFileName( const File& file )
    {
        if( filename_ == file ) return false;
        filename_ = file;
        return true;
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
        auto imagePath( _imagePath( id ) );
        auto files = { imagePath, File( imagePath + ".svg" ), File( imagePath + ".svgz" ) };
        for( auto file:files )
        { if( (file = file.addPath( path ) ).exists() ) return file; }

        return File();
    }

}

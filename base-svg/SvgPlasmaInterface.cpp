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
#include "Operators.h"
#include "SvgSystemOptions.h"
#include "Util.h"


#include <QSettings>

#include <algorithm>
#include <array>
#include <iterator>

namespace Svg
{

    //_________________________________________________
    SvgPlasmaInterface::SvgPlasmaInterface( QObject* parent ):
        QObject( parent )
    { installSvgSystemOptions(); }

    //_________________________________________________
    QPalette SvgPlasmaInterface::themePalette() const
    {

        QPalette out;

        QSettings settings( themePaletteFileName_, QSettings::IniFormat );
        settings.sync();

        auto updateColor = [&out,&settings](QPalette::ColorRole role, const QString &name)
        {
            if( settings.contains( name ) )
            {
                auto rgb( settings.value( name ).toStringList() );
                if( rgb.size() == 3 )
                { out.setColor( role, QColor( rgb[0].toInt(), rgb[1].toInt(), rgb[2].toInt() ) ); }
            }
        };

        updateColor( QPalette::WindowText, QStringLiteral("Colors:Window/ForegroundNormal") );
        updateColor( QPalette::Window, QStringLiteral("Colors:Window/BackgroundNormal") );
        updateColor( QPalette::Highlight, QStringLiteral("Colors:Selection/BackgroundNormal") );
        updateColor( QPalette::Text, QStringLiteral("Colors:View/ForegroundNormal") );
        updateColor( QPalette::Base, QStringLiteral("Colors:View/BackgroundNormal") );
        updateColor( QPalette::ButtonText, QStringLiteral("Colors:Button/ForegroundNormal") );
        updateColor( QPalette::Button, QStringLiteral("Colors:Button/BackgroundNormal") );
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
    bool SvgPlasmaInterface::loadTheme()
    {

        Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme" << Qt::endl;

        File::List configurationFiles;
        if( XmlOptions::get().contains( QStringLiteral("KDE_CONFIG") ) )
        {

            // get kde4 config command and retrieve output
            const QString kdeConfigCommand( XmlOptions::get().raw( QStringLiteral("KDE_CONFIG") ) );

            CustomProcess process( this );
            process.start( Base::Command( kdeConfigCommand ) << "--path" << "config" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                auto configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( QLatin1Char(':') );
                std::transform( configurationPath.begin(), configurationPath.end(), std::back_inserter( configurationFiles ),
                    []( const QString& path ) { return File( "plasmarc" ).addPath( File( path ) ); } );

            }

        }

        if( configurationFiles.isEmpty() )
        {
            // add some files manually in case the above failed
            configurationFiles.append( File( ".kde/share/config/plasmarc" ).addPath( Util::home() ) );
            configurationFiles.append( File( ".kde4/share/config/plasmarc" ).addPath( Util::home() ) );
        }

        // make sure fileSystemWatcher is valid
        if( !fileSystemWatcher_ )
        {
            fileSystemWatcher_ = new QFileSystemWatcher( this );
            connect( fileSystemWatcher_, &QFileSystemWatcher::fileChanged, this, &SvgPlasmaInterface::_configurationFileChanged );
        }

        // add valid configuration files to watcher
        auto oldFiles( fileSystemWatcher_->files() );
        for( const auto& file:configurationFiles )
        { if( file.exists() && !oldFiles.contains( file ) ) fileSystemWatcher_->addPath( file ); }

        // look for theme in selected configuration files
        QString theme( QStringLiteral("default") );
        for( const auto& file:configurationFiles )
        {
            if( !file.exists() ) continue;

            Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - checking: " << file << Qt::endl;

            // read group
            QSettings settings( file, QSettings::IniFormat );
            settings.sync();
            if( settings.contains( QStringLiteral("Theme/name") ) )
            {
                theme = settings.value( QStringLiteral("Theme/name") ).toString();
                break;
            }

        }

        Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - using theme: " << theme << Qt::endl;

        // reset colorFileName
        auto oldThemePaletteFileName = themePaletteFileName_;
        themePaletteFileName_.clear();

        // assign theme
        bool modified = _setTheme( theme );
        Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - using color scheme: " << themePaletteFileName_ << Qt::endl;

        // set as modified if themePaletteFile has changed
        modified |= (themePaletteFileName_ != oldThemePaletteFileName);
        Debug::Throw() << "Svg::SvgPlasmaInterface::loadTheme - modified: " << modified << Qt::endl;

        // return modified state
        return modified;

    }

    //_________________________________________________
    bool SvgPlasmaInterface::loadFile()
    {

        Debug::Throw( QStringLiteral("Svg::SvgPlasmaInterface::loadFile.\n") );

        // check path
        if( !path_.exists() ) return _setValid( false );

        File filename;
        const bool found =
            !( filename = _findImage( path_, imagePath_ ) ).isEmpty() ||
            !( filename = _findImage( path_, ImagePath::WidgetBackground ) ).isEmpty();

        // construct full filename base on theme and transparency setting
        bool changed( false );
        changed |= _setValid( found );
        changed |= _setFileName( found ? filename:File() );
        return changed;

    }

    //_________________________________________________
    void SvgPlasmaInterface::_configurationFileChanged( const QString& file )
    {
        Debug::Throw(0) << "Svg::SvgPlasmaInterface::_configurationFileChanged - file: " << file << Qt::endl;
        if( !timer_.isActive() ) timer_.start( 100, this );
    }

    //_________________________________________________
    void SvgPlasmaInterface::timerEvent( QTimerEvent* event )
    {
        if( event->timerId() == timer_.timerId() )
        {

            Debug::Throw(0, QStringLiteral("Svg::SvgPlasmaInterface::timerEvent.\n") );
            timer_.stop();
            emit themeChanged();

        } else return QObject::timerEvent( event );
    }

    //_________________________________________________
    bool SvgPlasmaInterface::_setTheme( const QString& theme )
    {

        Debug::Throw() << "Svg::SvgPlasmaInterface::_setTheme - theme:" << theme << Qt::endl;
        File::List themePathList;

        if( XmlOptions::get().contains( QStringLiteral("KDE_CONFIG") ) )
        {

            // get kde4 config command and retrieve output
            const QString kdeConfigCommand( XmlOptions::get().raw( QStringLiteral("KDE_CONFIG") ) );

            CustomProcess process( this );
            process.start( Base::Command( kdeConfigCommand ) << "--path" << "data" );
            if( process.waitForFinished() && process.exitStatus() == QProcess::NormalExit )
            {
                const auto configurationPath = QString( process.readAllStandardOutput() ).trimmed().split( QLatin1Char(':') );
                std::transform( configurationPath.begin(), configurationPath.end(), std::back_inserter( themePathList ),
                    [&theme]( const QString& path ) { return File( theme ).addPath( File( "plasma/desktoptheme/" ).addPath( File( path ) ) ); }
                    );

                // also add kde4 path
                std::transform( configurationPath.begin(), configurationPath.end(), std::back_inserter( themePathList ),
                    [&theme]( const QString& path ) { return File( theme ).addPath( File( "desktoptheme/" ).addPath( File( path ) ) ); }
                    );

            }

        }

        if( themePathList.empty() )
        {
            // add local path
            static const std::array<QString,2> localPath = {{ ".kde/share/apps/desktoptheme/", ".kde4/share/apps/desktoptheme/" }};
            std::transform( localPath.begin(), localPath.end(), std::back_inserter( themePathList ),
                [&theme]( const QString& pathName ) { return File( theme ).addPath( File( pathName ).addPath( Util::home() ) ); } );
        }

        // add local path
        themePathList.append( File( theme ).addPath( File( "/usr/share/apps/desktoptheme/" ) ) );
        for( const auto& themePath:themePathList )
        {
            Debug::Throw() << "Svg::SvgPlasmaInterface::_setTheme - checking " << themePath << Qt::endl;
            if( themePath.exists() )
            {

                // try load color palette
                if( themePaletteFileName_.isEmpty() )
                {
                    File file = File( "colors" ).addPath( themePath );
                    if( file.exists() ) themePaletteFileName_ = file;
                }

                if( !_findImage( themePath, ImagePath::WidgetBackground ).isEmpty() )
                { return _setPath( themePath ); }

            }

        }

        // try use default theme
        if( theme == QLatin1String("default") )
        {

            bool changed( false );
            changed |= _setPath( File() );
            changed |= _setValid( false );
            return changed;

        } else return _setTheme( QStringLiteral("default") );

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
            case ImagePath::WidgetBackground: return File( "widgets/background" );
            case ImagePath::DialogBackground: return File( "dialogs/background" );
            case ImagePath::WidgetTranslucentBackground: return  File( "widgets/translucentbackground" );
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
        { if( file.addPath( path ).exists() ) return file; }

        return File();
    }

}

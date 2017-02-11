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

#include "DefaultFolders.h"

#include "Debug.h"
#include "Util.h"

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include <QFileInfo>
#include <QIcon>
#include <QSettings>

#if defined( Q_OS_WIN )
#include <windows.h>
#include <shlobj.h>
#endif

//__________________________________________________________________________
DefaultFolders& DefaultFolders::get( void )
{
    static DefaultFolders singleton_;
    return singleton_;
}

//__________________________________________________________________________
DefaultFolders::DefaultFolders( void )
{

    // fill folder map
    folders_.insert( Util::home(), Home );

    #if QT_VERSION >= 0x050000
    _insert( QStandardPaths::writableLocation( QStandardPaths::DesktopLocation ), Desktop );
    _insert( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), Documents );
    _insert( QStandardPaths::writableLocation( QStandardPaths::MusicLocation ), Music );
    _insert( QStandardPaths::writableLocation( QStandardPaths::PicturesLocation ), Pictures );
    _insert( QStandardPaths::writableLocation( QStandardPaths::TempLocation ), Templates );
    _insert( QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ), Videos );
    #else
    _insert( QDesktopServices::storageLocation( QDesktopServices::DesktopLocation ), Desktop );
    _insert( QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation ), Documents );
    _insert( QDesktopServices::storageLocation( QDesktopServices::MusicLocation ), Music );
    _insert( QDesktopServices::storageLocation( QDesktopServices::PicturesLocation ), Pictures );
    _insert( QDesktopServices::storageLocation( QDesktopServices::TempLocation ), Templates );
    _insert( QDesktopServices::storageLocation( QDesktopServices::MoviesLocation ), Videos );
    #endif

    #if defined( Q_OS_LINUX )
    // use QSettings to get standard directories from XDG
    QSettings settings( QString( "%1/.config/user-dirs.dirs" ).arg( Util::home() ), QSettings::IniFormat );
    settings.sync();
    _insert( settings.value( "XDG_DOWNLOAD_DIR", "$HOME/Downloads" ).value<QString>().replace( "$HOME", Util::home() ), Downloads );
    #endif

}

//__________________________________________________________________________
void DefaultFolders::initializeFolderNames( void )
{
    if( folderNamesInitialized_ ) return;

    // fill icons map
    names_.insert( Home, tr( "Home" ) );
    names_.insert( Desktop, tr( "Desktop" ) );
    names_.insert( Documents, tr( "Documents" ) );
    names_.insert( Downloads, tr( "Downloads" ) );
    names_.insert( Music, tr( "Music" ) );
    names_.insert( Pictures, tr( "Pictures" ) );
    names_.insert( Templates, tr( "Templates" ) );
    names_.insert( Videos, tr( "Video" ) );

    folderNamesInitialized_ = true;
}

//__________________________________________________________________________
void DefaultFolders::initializeIconNames( void )
{
    if( iconNamesInitialized_ ) return;

    // fill icons map
    iconNames_.insert( Home, "user-home.png" );
    iconNames_.insert( Desktop, "user-desktop" );
    iconNames_.insert( Documents, "folder-documents" );
    iconNames_.insert( Music, "folder-sound" );

    // there are discrepencies for folder icon names between icon themes
    // try to deal with major ones
    if( QIcon::hasThemeIcon( "folder-download" ) ) iconNames_.insert( Downloads, "folder-download" );
    else iconNames_.insert( Downloads, "folder-downloads" );

    if( QIcon::hasThemeIcon( "folder-pictures" ) ) iconNames_.insert( Pictures, "folder-pictures" );
    else iconNames_.insert( Pictures, "folder-image" );

    if( QIcon::hasThemeIcon( "folder-videos" ) ) iconNames_.insert( Videos, "folder-videos" );
    else iconNames_.insert( Videos, "folder-video" );

    iconNamesInitialized_ = true;
}

//__________________________________________________________________________
DefaultFolders::Type DefaultFolders::type( const File& file ) const
{
    auto iter( allFolders_.find( file ) );
    return iter == allFolders_.end() ? Unknown:iter.value();
}

//__________________________________________________________________________
File DefaultFolders::file( DefaultFolders::Type type ) const
{
    for( auto iter = folders_.constBegin(); iter != folders_.constEnd(); ++iter )
    { if( iter.value() == type ) return iter.key(); }

    return File();
}

//__________________________________________________________________________
QString DefaultFolders::name( Type type ) const
{
    auto iter( names_.find( type ) );
    return iter == names_.end() ? QString():iter.value();
}

//__________________________________________________________________________
QString DefaultFolders::iconName( Type type ) const
{
    auto iter( iconNames_.find( type ) );
    return iter == iconNames_.end() ? QString():iter.value();
}

//__________________________________________________________________________
void DefaultFolders::_insert( const QStringList& keys, Type value )
{
    for( const auto& key:keys ) allFolders_.insert( key, value );
    if( !keys.isEmpty() ) folders_.insert( keys.front(), value );
}

//__________________________________________________________________________
void DefaultFolders::_insert( const QString& key, Type value )
{
    if( !key.isEmpty() )
    {
        allFolders_.insert( key, value );
        folders_.insert( key, value );
    }
}

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
#include "Util.h"

#include <QFileInfo>
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

    #if defined( Q_OS_LINUX )

    // use QSettings to get standard directories from XDG
    QSettings settings( QString( "%1/.config/user-dirs.dirs" ).arg( Util::home() ), QSettings::IniFormat );
    settings.sync();
    _insert( settings.value( "XDG_DESKTOP_DIR", "$HOME/Desktop" ).value<QString>().replace( "$HOME", Util::home() ), Desktop );
    _insert( settings.value( "XDG_DOCUMENTS_DIR", "$HOME/Documents" ).value<QString>().replace( "$HOME", Util::home() ), Documents );
    _insert( settings.value( "XDG_DOWNLOAD_DIR", "$HOME/Downloads" ).value<QString>().replace( "$HOME", Util::home() ), Downloads );
    _insert( settings.value( "XDG_MUSIC_DIR", "$HOME/Music" ).value<QString>().replace( "$HOME", Util::home() ), Music );
    _insert( settings.value( "XDG_PICTURES_DIR", "$HOME/Pictures" ).value<QString>().replace( "$HOME", Util::home() ), Pictures );
    _insert( settings.value( "XDG_TEMPLATES_DIR", "$HOME/Templates" ).value<QString>().replace( "$HOME", Util::home() ), Templates );
    _insert( settings.value( "XDG_VIDEOS_DIR", "$HOME/Videos" ).value<QString>().replace( "$HOME", Util::home() ), Videos );

    #elif defined( Q_OS_WIN )

    // use windows API to get standard directories
    _insert( _defaultFolderName( CSIDL_DESKTOPDIRECTORY ), Desktop );
    _insert( _defaultFolderName( CSIDL_PERSONAL ), Documents );
    _insert( _defaultFolderName( CSIDL_MYMUSIC ), Music );
    _insert( _defaultFolderName( CSIDL_MYPICTURES ), Pictures );
    _insert( _defaultFolderName( CSIDL_MYVIDEO ), Videos );

    #endif

    // fill icons map
    names_.insert( Home, tr( "Home" ) );
    names_.insert( Desktop, tr( "Desktop" ) );
    names_.insert( Documents, tr( "Documents" ) );
    names_.insert( Downloads, tr( "Downloads" ) );
    names_.insert( Music, tr( "Music" ) );
    names_.insert( Pictures, tr( "Pictures" ) );
    names_.insert( Templates, tr( "Templates" ) );
    names_.insert( Videos, tr( "Video" ) );

    // fill icons map
    iconNames_.insert( Home, "user-home.png" );
    iconNames_.insert( Desktop, "user-desktop" );
    iconNames_.insert( Documents, "folder-documents" );
    iconNames_.insert( Downloads, "folder-downloads" );
    iconNames_.insert( Music, "folder-sound" );
    iconNames_.insert( Pictures, "folder-image" );
    iconNames_.insert( Videos, "folder-video" );

}

//__________________________________________________________________________
DefaultFolders::Type DefaultFolders::type( const File& file ) const
{
    FolderMap::const_iterator iter = folders_.find( file );
    return iter == folders_.end() ? Unknown:iter.value();
}

//__________________________________________________________________________
QString DefaultFolders::name( Type type ) const
{
    IconMap::const_iterator iter = names_.find( type );
    return iter == names_.end() ? QString():iter.value();
}

//__________________________________________________________________________
QString DefaultFolders::iconName( Type type ) const
{
    IconMap::const_iterator iter = iconNames_.find( type );
    return iter == iconNames_.end() ? QString():iter.value();
}

//__________________________________________________________________________
QString DefaultFolders::_defaultFolderName( qint64 key )
{
    #if defined( Q_OS_WIN )
    TCHAR returnPath[MAX_PATH];
    if( SUCCEEDED( SHGetFolderPath( 0, key, 0, 0, returnPath ) ) ) return QFileInfo( returnPath ).canonicalFilePath();
    else return QString();
    #else
    return QString();
    #endif
}

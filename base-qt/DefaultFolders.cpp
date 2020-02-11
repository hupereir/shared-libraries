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
#include "CppUtil.h"
#include "Debug.h"
#include "Util.h"

#include <QStandardPaths>

#include <QFileInfo>
#include <QIcon>
#include <QSettings>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <shlobj.h>
#endif

//__________________________________________________________________________
DefaultFolders& DefaultFolders::get()
{
    static DefaultFolders singleton_;
    return singleton_;
}

//__________________________________________________________________________
DefaultFolders::DefaultFolders():
    names_(
    {
        { Type::Home, tr( "Home" ) },
        { Type::Desktop, tr( "Desktop" ) },
        { Type::Documents, tr( "Documents" ) },
        { Type::Downloads, tr( "Downloads" ) },
        { Type::Music, tr( "Music" ) },
        { Type::Pictures, tr( "Pictures" ) },
        { Type::Templates, tr( "Templates" ) },
        { Type::Videos, tr( "Video" ) }
    }),
    iconNames_(
    {
        { Type::Home, "user-home.png" },
        { Type::Desktop, "user-desktop" },
        { Type::Documents, "folder-documents" },
        { Type::Music, "folder-sound" }
    })
{

    // there are discrepencies for folder icon names between icon themes
    // try to deal with major ones
    if( QIcon::hasThemeIcon( QStringLiteral("folder-download") ) ) iconNames_.insert( Type::Downloads, QStringLiteral("folder-download") );
    else iconNames_.insert( Type::Downloads, QStringLiteral("folder-downloads") );

    if( QIcon::hasThemeIcon( QStringLiteral("folder-pictures") ) ) iconNames_.insert( Type::Pictures, QStringLiteral("folder-pictures") );
    else iconNames_.insert( Type::Pictures, QStringLiteral("folder-image") );

    if( QIcon::hasThemeIcon( QStringLiteral("folder-videos") ) ) iconNames_.insert( Type::Videos, QStringLiteral("folder-videos") );
    else iconNames_.insert( Type::Videos, QStringLiteral("folder-video") );

    // path map
    folders_.insert( Util::home(), Type::Home );

    _insert( QStandardPaths::writableLocation( QStandardPaths::DesktopLocation ), Type::Desktop );
    _insert( QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation ), Type::Documents );
    _insert( QStandardPaths::writableLocation( QStandardPaths::MusicLocation ), Type::Music );
    _insert( QStandardPaths::writableLocation( QStandardPaths::PicturesLocation ), Type::Pictures );
    _insert( QStandardPaths::writableLocation( QStandardPaths::TempLocation ), Type::Templates );
    _insert( QStandardPaths::writableLocation( QStandardPaths::MoviesLocation ), Type::Videos );

    #if defined( Q_OS_LINUX )
    // use QSettings to get standard directories from XDG
    QSettings settings( QStringLiteral( "%1/.config/user-dirs.dirs" ).arg( Util::home() ), QSettings::IniFormat );
    settings.sync();
    _insert( settings.value( QStringLiteral("XDG_DOWNLOAD_DIR"), "$HOME/Downloads" ).value<QString>().replace( QLatin1String("$HOME"), Util::home() ), Type::Downloads );
    #endif

}

//__________________________________________________________________________
DefaultFolders::Type DefaultFolders::type( const File& file ) const
{
    auto iter( allFolders_.find( file ) );
    return iter == allFolders_.end() ? Type::Unknown:iter.value();
}

//__________________________________________________________________________
File DefaultFolders::file( DefaultFolders::Type type ) const
{
    for( auto&& iter = folders_.constBegin(); iter != folders_.constEnd(); ++iter )
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
    for( const auto& key:keys ) allFolders_.insert( File( key ), value );
    if( !keys.isEmpty() ) Base::insert( folders_, File( keys.front() ), value );
}

//__________________________________________________________________________
void DefaultFolders::_insert( const QString& key, Type value )
{
    if( !key.isEmpty() )
    {
        allFolders_.insert( File( key ), value );
        Base::insert( folders_, File( key ), value );
    }
}

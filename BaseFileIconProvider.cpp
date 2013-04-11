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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "BaseFileIconProvider.h"
#include "BaseIcons.h"
#include "CustomPixmap.h"
#include "IconEngine.h"
#include "Util.h"

#include <QSettings>

//__________________________________________________________________________
BaseFileIconProvider::BaseFileIconProvider( QObject* parent ):
    QObject( parent ),
    Counter( "BaseFileIconProvider" )
{

    // initialize default icons
    // home
    icons_.insert( Util::home(), Icon("user-home.png") );

    // use QSettings to get standard directories from XDG
    QSettings settings( QString( "%1/.config/user-dirs.dirs" ).arg( Util::home() ), QSettings::IniFormat );
    settings.sync();
    icons_.insert( settings.value( "XDG_DESKTOP_DIR", "$HOME/Desktop" ).value<QString>().replace( "$HOME", Util::home() ), Icon("user-desktop") );
    icons_.insert( settings.value( "XDG_DOCUMENTS_DIR", "$HOME/Documents" ).value<QString>().replace( "$HOME", Util::home() ), Icon("folder-documents") );
    icons_.insert( settings.value( "XDG_DOWNLOAD_DIR", "$HOME/Downloads" ).value<QString>().replace( "$HOME", Util::home() ), Icon("folder-downloads") );
    icons_.insert( settings.value( "XDG_MUSIC_DIR", "$HOME/Music" ).value<QString>().replace( "$HOME", Util::home() ), Icon("folder-sound") );
    icons_.insert( settings.value( "XDG_PICTURES_DIR", "$HOME/Pictures" ).value<QString>().replace( "$HOME", Util::home() ), Icon("folder-image") );
    icons_.insert( settings.value( "XDG_VIDEOS_DIR", "$HOME/Videos" ).value<QString>().replace( "$HOME", Util::home() ), Icon("folder-video") );

}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::icon( const BaseFileInfo& fileInfo )
{

    // get type
    const int type( fileInfo.type() );
    if( !( type&BaseFileInfo::Folder ) ) return IconEngine::get( QString() );

    // find in predefined icons
    const IconMap::iterator iter( icons_.find( fileInfo.file() ) );
    return iter == icons_.end() ? IconEngine::get( QString() ) : iter->icon( type&BaseFileInfo::Link );

}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::Icon::icon( bool isLink )
{

    // initialize icon if null
    if( icon_.isNull() )
    {
        QIcon linkOverlay( IconEngine::get( ICONS::LINK ) );
        QIcon copy( IconEngine::get( name_ ) );
        foreach( const QSize& size, copy.availableSizes() )
        {
            if( isLink )
            {

                QSize overlaySize;
                if( size.width() <= 16 ) overlaySize = QSize( 10, 10 );
                else if( size.width() <= 22 ) overlaySize = QSize( 12, 12 );
                else if( size.width() <= 32 ) overlaySize = QSize( 16, 16 );
                else if( size.width() <= 48 ) overlaySize = QSize( 16, 16 );
                else if( size.width() <= 64 ) overlaySize = QSize( 22, 22 );
                else if( size.width() <= 128 ) overlaySize = QSize( 48, 48 );
                else overlaySize = QSize( 64, 64 );

                icon_.addPixmap( CustomPixmap( copy.pixmap( size ) ).merge( linkOverlay.pixmap( overlaySize ).scaled( overlaySize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ), CustomPixmap::BOTTOM_RIGHT ) );

            } else icon_.addPixmap( copy.pixmap( size ) );
        }
    }

    return icon_;
}

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
#include "DefaultFolders.h"
#include "IconEngine.h"
#include "Util.h"

#include <QSettings>

//__________________________________________________________________________
BaseFileIconProvider::BaseFileIconProvider( QObject* parent ):
    QObject( parent ),
    Counter( "BaseFileIconProvider" )
{

    const DefaultFolders::FolderMap& folders( DefaultFolders::get().folders() );

    for( DefaultFolders::FolderMap::const_iterator iter = folders.begin(); iter != folders.end(); ++iter )
    {
        const QString iconName( DefaultFolders::get().iconName( iter.value() ) );
        if( !iconName.isEmpty() ) icons_.insert( iter.key(), Icon( iconName ) );
    }

}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::icon( const BaseFileInfo& fileInfo, int type )
{

    // get type
    if( !( type&BaseFileInfo::Folder ) ) return IconEngine::get( QString() );

    // find in predefined icons
    const IconMap::iterator iter( icons_.find( fileInfo.file() ) );
    return iter == icons_.end() ? IconEngine::get( QString() ) : iter->icon( type&BaseFileInfo::Link );

}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::Icon::icon( bool isLink )
{

    QIcon& icon = isLink ? linkIcon_:icon_;

    // initialize icon if null
    if( icon.isNull() )
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

                icon.addPixmap( CustomPixmap( copy.pixmap( size ) ).merge( linkOverlay.pixmap( overlaySize ).scaled( overlaySize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ), CustomPixmap::BOTTOM_RIGHT ) );

            } else icon.addPixmap( copy.pixmap( size ) );
        }
    }

    return icon;
}

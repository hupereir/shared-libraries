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
{}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::icon( const BaseFileInfo& fileInfo, int type )
{

    // get type
    if( !( type&BaseFileInfo::Folder ) ) return invalid_;

    // check whether current map contains key
    IconMap::const_iterator iter( icons_.find( Key( fileInfo.file(), type ) ) );
    if( iter != icons_.end() ) return iter.value();

    // insert new entry in map
    const DefaultFolders::FolderMap& folders( DefaultFolders::get().folders() );
    DefaultFolders::FolderMap::const_iterator nameIter( folders.find( fileInfo.file() ) );
    if( nameIter == folders.end() ) return invalid_;

    // get icon name and corresponding icon from engine
    const QString iconName( DefaultFolders::get().iconName( nameIter.value() ) );
    const QIcon& base( IconEngine::get( iconName ) );
    if( base.isNull() ) return invalid_;

    // link overlay
    QIcon copy;
    QIcon linkOverlay( IconEngine::get( ICONS::LINK ) );

    // add relevant sizes
    foreach( const QSize& size, base.availableSizes() )
    {

        CustomPixmap pixmap;
        if( type&BaseFileInfo::Link )
        {

            QSize overlaySize;
            if( size.width() <= 16 ) overlaySize = QSize( 10, 10 );
            else if( size.width() <= 22 ) overlaySize = QSize( 12, 12 );
            else if( size.width() <= 32 ) overlaySize = QSize( 16, 16 );
            else if( size.width() <= 48 ) overlaySize = QSize( 16, 16 );
            else if( size.width() <= 64 ) overlaySize = QSize( 22, 22 );
            else if( size.width() <= 128 ) overlaySize = QSize( 48, 48 );
            else overlaySize = QSize( 64, 64 );

            pixmap = CustomPixmap( base.pixmap( size ) ).merge( linkOverlay.pixmap( overlaySize ).scaled( overlaySize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation ), CustomPixmap::BOTTOM_RIGHT );

        } else pixmap = base.pixmap( size );

        // disable icon, for clipped
        if( type & BaseFileInfo::Clipped )
        { pixmap = pixmap.desaturate().transparent( 0.6 ); }

        // add to icon
        copy.addPixmap( pixmap );
    }

    // insert in map and return
    return icons_.insert( Key( fileInfo.file(), type ), copy ).value();

}

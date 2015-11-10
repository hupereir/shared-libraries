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
#include "BaseIconNames.h"
#include "CustomPixmap.h"
#include "DefaultFolders.h"
#include "IconEngine.h"
#include "Util.h"

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
    IconCache::const_iterator iter( icons_.find( Key( fileInfo.file(), type ) ) );
    if( iter != icons_.end() ) return iter.value();

    // insert new entry in map
    const DefaultFolders::FolderMap& folders( DefaultFolders::get().folders() );
    DefaultFolders::FolderMap::const_iterator nameIter( folders.find( fileInfo.file() ) );
    if( nameIter == folders.end() ) return invalid_;

    // get icon name
    const QString iconName( DefaultFolders::get().iconName( nameIter.value() ) );
    if( iconName.isEmpty() ) return invalid_;

    // get corresponding icon from icon engine
    const QIcon& base( IconEngine::get( iconName ) );
    if( base.isNull() ) return invalid_;

    // insert in map and return
    return icons_.insert( Key( fileInfo.file(), type ), base ).value();

}


//____________________________________________________
QPixmap BaseFileIconProvider::linked( const CustomPixmap& source )
{

    if( source.isNull() ) return source;
    QIcon linkOverlay( IconEngine::get( IconNames::SymbolicLink ) );
    if( linkOverlay.isNull() ) return source;

    // get source size
    const QSize size( source.size()/source.devicePixelRatio() );

    // decide overlay size
    QSize overlaySize;
    if( size.width() <= 16 ) overlaySize = QSize( 10, 10 );
    else if( size.width() <= 22 ) overlaySize = QSize( 12, 12 );
    else if( size.width() <= 32 ) overlaySize = QSize( 16, 16 );
    else if( size.width() <= 48 ) overlaySize = QSize( 16, 16 );
    else if( size.width() <= 64 ) overlaySize = QSize( 22, 22 );
    else if( size.width() <= 128 ) overlaySize = QSize( 48, 48 );
    else overlaySize = QSize( 64, 64 );

    return source.merge( linkOverlay.pixmap( overlaySize )
        .scaled( overlaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation ), CustomPixmap::BOTTOM_RIGHT );

}

//____________________________________________________
QPixmap BaseFileIconProvider::hidden( const CustomPixmap& source )
{ return source.transparent( 0.6 ); }

//____________________________________________________
QPixmap BaseFileIconProvider::clipped( const CustomPixmap& source )
{ return source.desaturate().transparent( 0.6 ); }

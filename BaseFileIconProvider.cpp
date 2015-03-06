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
    IconMap::const_iterator iter( icons_.find( Key( fileInfo.file(), type ) ) );
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

    // link overlay
    QIcon copy;
    QIcon linkOverlay( IconEngine::get( IconNames::SymbolicLink ) );

    // add relevant sizes
    foreach( const QSize& size, base.availableSizes() )
    {

        CustomPixmap pixmap = type&BaseFileInfo::Link ? _linked( CustomPixmap( base.pixmap( size ) ) ):base.pixmap( size );

        // add clipped effect
        if( type & BaseFileInfo::Clipped ) pixmap = _clipped( pixmap );

        // add to icon
        copy.addPixmap( pixmap );
    }

    // insert in map and return
    return icons_.insert( Key( fileInfo.file(), type ), copy ).value();

}

//____________________________________________________
QPixmap BaseFileIconProvider::_linked( const CustomPixmap& source ) const
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
QPixmap BaseFileIconProvider::_hidden( const CustomPixmap& source ) const
{ return source.transparent( 0.6 ); }

//____________________________________________________
QPixmap BaseFileIconProvider::_clipped( const CustomPixmap& source ) const
{ return source.desaturate().transparent( 0.6 ); }

//____________________________________________________
QIcon BaseFileIconProvider::_linked( const QIcon& source ) const
{
    QIcon destination;
    foreach( const QSize& size, source.availableSizes() )
    { destination.addPixmap( _linked( CustomPixmap( source.pixmap( size ) ) ) ); }
    return destination;
}

//____________________________________________________
QIcon BaseFileIconProvider::_hidden( const QIcon& source ) const
{
    QIcon destination;
    foreach( const QSize& size, source.availableSizes() )
    { destination.addPixmap( _hidden( CustomPixmap( source.pixmap( size ) ) ) ); }
    return destination;
}

//____________________________________________________
QIcon BaseFileIconProvider::_clipped( const QIcon& source ) const
{
    QIcon destination;
    foreach( const QSize& size, source.availableSizes() )
    { destination.addPixmap( _clipped( CustomPixmap( source.pixmap( size ) ) ) ); }
    return destination;
}

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
#include "Pixmap.h"
#include "DefaultFolders.h"
#include "IconEngine.h"
#include "Util.h"

//__________________________________________________________________________
BaseFileIconProvider::BaseFileIconProvider( QObject* parent ):
    QObject( parent ),
    Counter( QStringLiteral("BaseFileIconProvider") )
{}

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::icon( const BaseFileInfo& fileInfo, int type )
{

    // get type
    if( !( type&BaseFileInfo::Folder ) ) return invalid_;

    // check whether current map contains key
    auto&& iter( icons_.find( Key( fileInfo.file(), type ) ) );
    if( iter != icons_.end() ) return iter.value();

    // insert new entry in map
    auto&& folderType( DefaultFolders::get().type( fileInfo.file() ) );
    if( folderType == DefaultFolders::Type::Unknown ) return invalid_;

    // get icon name
    auto&& iconName( DefaultFolders::get().iconName( folderType ) );
    if( iconName.isEmpty() ) return invalid_;

    // get corresponding icon from icon engine
    auto&& base( IconEngine::get( iconName ) );
    if( base.isNull() ) return invalid_;

    // insert in map and return
    return icons_.insert( Key( fileInfo.file(), type ), base ).value();

}


//____________________________________________________
Pixmap BaseFileIconProvider::linked( const Pixmap& source )
{

    if( source.isNull() ) return source;
    auto linkOverlay( IconEngine::get( IconNames::SymbolicLink ) );
    if( linkOverlay.isNull() ) return source;

    // get source size
    auto size( source.size()/source.devicePixelRatio() );

    // decide overlay size
    QSize overlaySize;
    if( size.width() <= 16 ) overlaySize = QSize( 10, 10 );
    else if( size.width() <= 22 ) overlaySize = QSize( 12, 12 );
    else if( size.width() <= 32 ) overlaySize = QSize( 16, 16 );
    else if( size.width() <= 48 ) overlaySize = QSize( 16, 16 );
    else if( size.width() <= 64 ) overlaySize = QSize( 22, 22 );
    else if( size.width() <= 128 ) overlaySize = QSize( 48, 48 );
    else overlaySize = QSize( 64, 64 );

    Pixmap overlayPixmap( linkOverlay.pixmap( overlaySize ) );
    return source.merged(
        overlayPixmap.scaled( overlaySize*overlayPixmap.devicePixelRatio(), Qt::KeepAspectRatio, Qt::SmoothTransformation ),
        Pixmap::Corner::BottomRight );

}

//____________________________________________________
Pixmap BaseFileIconProvider::hidden( const Pixmap& source )
{ return source.transparent( 0.6 ); }

//____________________________________________________
Pixmap BaseFileIconProvider::clipped( const Pixmap& source )
{ return source.desaturated().transparent( 0.6 ); }

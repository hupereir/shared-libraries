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

#include "IconEngine.h"
#include "XmlOptions.h"

#include <QFileInfo>

//__________________________________________________________
IconEngine& IconEngine::get()
{
    //* singleton
    static IconEngine singleton_;
    return singleton_;
}

//__________________________________________________________
IconEngine::IconEngine():
    Counter( QStringLiteral("IconEngine") )
{ Debug::Throw( QStringLiteral("IconEngine::IconEngine.\n") ); }

//__________________________________________________________
bool IconEngine::reload()
{
    Debug::Throw( QStringLiteral("IconEngine::reload.\n") );

    // load path from options
    auto pathList( XmlOptions::get().specialOptions<File>( "PIXMAP_PATH" ) );
    if( pathList == pixmapPath_ ) return false;

    pixmapPath_ = pathList;

    for( auto&& iter = cache_.begin(); iter != cache_.end(); ++iter )
    { cache_[iter.key()] = _get( iter.key(), iter.value().flags()&~static_cast<Base::IconCacheItem::Flags>(Base::IconCacheItem::Flag::FromCache) ); }

    return true;
}

//__________________________________________________________
void IconEngine::clear()
{
    Debug::Throw( QStringLiteral("IconEngine::clear.\n") );
    cache_.clear();
}

//__________________________________________________________
const Base::IconCacheItem& IconEngine::_get( const QString& file, Base::IconCacheItem::Flags flags )
{

    // try find file in cache
    if( flags & Base::IconCacheItem::Flag::FromCache )
    {
        Base::IconCache::iterator iter( cache_.find( file ) );
        if( iter != cache_.end() ) return iter.value();
    }

    // debug
    Debug::Throw() << "IconEngine::_get - file: " << file << endl;

    // insert null icon for empty filename
    Base::IconCacheItem out;
    out.setFlags( flags );
    if( file.isEmpty() )
    {

        out.addFile( file );

    } else if( QFileInfo( file ).isAbsolute() ) {

        out.QIcon::addFile( file );
        out.addFile( file );

    } else {

        const auto truncatedName( File( file ).truncatedName() );
        if( (flags&Base::IconCacheItem::Flag::FromTheme) && QIcon::hasThemeIcon( truncatedName ) )
        {

            out = Base::IconCacheItem( QIcon::fromTheme( truncatedName ) );
            out.addFile( QObject::tr( "from theme" ) );

        } else {

            // make sure pixmap path is initialized
            if( pixmapPath_.empty() ) pixmapPath_ = XmlOptions::get().specialOptions<File>( "PIXMAP_PATH" );

            // store list of loaded sizes
            QList<QSize> sizes;
            for( const auto& path:pixmapPath_ )
            {

                // skip empty path
                if( path.isEmpty() ) continue;

                // prepare filename
                File pixmapFile;

                // see if path is internal resource path
                if( path.startsWith( ':' ) ) pixmapFile = File( file ).addPath( path );
                else pixmapFile = File( path ).find( File( file ) );

                // load pixmap
                if( pixmapFile.isEmpty() ) continue;

                QPixmap pixmap( pixmapFile );
                if( pixmap.isNull() ) continue;

                // check size
                if( sizes.contains( pixmap.size() ) ) continue;
                out.addPixmap( pixmap );
                out.addFile( pixmapFile );
                sizes.append( pixmap.size() );

            }

        }

    }

    return cache_.insert( file, out ).value();

}

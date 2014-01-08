
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

#include "IconEngine.h"
#include "XmlOptions.h"

#include <QFileInfo>

//__________________________________________________________
IconEngine& IconEngine::get( void )
{
    //! singleton
    static IconEngine singleton_;
    return singleton_;
}

//__________________________________________________________
IconEngine::IconEngine( void ):
    Counter( "IconEngine" )
{ Debug::Throw( "IconEngine::IconEngine.\n" ); }

//__________________________________________________________
bool IconEngine::reload( void )
{
    Debug::Throw( "IconEngine::reload.\n" );

    // load path from options
    QStringList pathList( XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" ) );
    if( pathList == pixmapPath_ ) return false;

    pixmapPath_ = pathList;

    for( BASE::IconCache::iterator iter = cache_.begin(); iter != cache_.end(); ++iter )
    { cache_[iter.key()] = _get( iter.key(), false ); }

    return true;
}

//__________________________________________________________
const BASE::IconCacheItem& IconEngine::_get( const QString& file, bool fromCache )
{

    // try find file in cache
    if( fromCache )
    {
        BASE::IconCache::iterator iter( cache_.find( file ) );
        if( iter != cache_.end() ) return iter.value();
    }

    // debug
    Debug::Throw() << "IconEngine::_get - file: " << file << endl;

    // insert null icon for empty filename
    BASE::IconCacheItem out;
    if( file.isEmpty() )
    {

        out.addFile( file );

    } else if( QFileInfo( file ).isAbsolute() ) {

        out.addPixmap( QPixmap( file ) );
        out.addFile( file );

    } else {

        #if QT_VERSION >= 0x040600
        QString truncatedName( File( file ).truncatedName() );
        if( QIcon::hasThemeIcon( truncatedName ) )
        {

            out = QIcon::fromTheme( truncatedName );
            out.addFile( QObject::tr( "from theme" ) );

        } else
        #endif
        {

            // make sure pixmap path is initialized
            if( pixmapPath_.empty() ) pixmapPath_ = XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" );

            // store list of loaded sizes
            QList<QSize> sizes;
            foreach( const QString& path, pixmapPath_ )
            {

                // skip empty path
                if( path.isEmpty() ) continue;

                // prepare filename
                File pixmapFile;

                // see if path is internal resource path
                if( path.startsWith( ':' ) ) pixmapFile = File( file ).addPath( path );
                else pixmapFile = File( path ).find( file );

                // load pixmap
                if( pixmapFile.isEmpty() ) continue;

                QPixmap pixmap( pixmapFile );
                if( pixmap.isNull() ) continue;

                // check size
                if( sizes.contains( pixmap.size() ) ) continue;
                out.addPixmap( pixmap );
                out.addFile( pixmapFile );
                sizes << pixmap.size();

            }

        }

    }

    return cache_.insert( file, out ).value();

}

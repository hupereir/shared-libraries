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

#include "File.h"
#include "PixmapEngine.h"
#include "XmlOptions.h"

#include <QFileInfo>

//__________________________________________________________
PixmapEngine& PixmapEngine::get()
{
    //! singleton
    static PixmapEngine singleton_;
    return singleton_;
}

//__________________________________________________________
PixmapEngine::PixmapEngine():
    Counter( QStringLiteral("PixmapEngine") )
{ Debug::Throw( QStringLiteral("PixmapEngine::PixmapEngine.\n") ); }

//__________________________________________________________
bool PixmapEngine::reload()
{
    Debug::Throw( QStringLiteral("PixmapEngine::reload.\n") );

    // load path from options
    auto pathList( XmlOptions::get().specialOptions<File>( "PIXMAP_PATH" ) );
    if( pathList == pixmapPath_ ) return false;

    pixmapPath_ = pathList;
    for( auto&& iter = cache_.begin(); iter != cache_.end(); ++iter )
    { cache_[iter.key()] = _get( iter.key(), false ); }

    return true;
}

//__________________________________________________________
const QPixmap& PixmapEngine::_get( const QString& file, bool fromCache )
{
    Debug::Throw( QStringLiteral("PixmapEngine::_get (file).\n") );

    // try find file in cache
    if( fromCache )
    {
        Base::PixmapCache::iterator iter( cache_.find( file ) );
        if( iter != cache_.end() ) return iter.value();
    }

    // create output
    QPixmap out;
    if( QFileInfo( file ).isAbsolute() ) { out = QPixmap( file ); }
    else {

        if( pixmapPath_.empty() ) pixmapPath_ = XmlOptions::get().specialOptions<File>( "PIXMAP_PATH" );
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
            if( !pixmapFile.isEmpty() )
            {
                out.load( pixmapFile );
                if( !out.isNull() ) break;
            }

        }

    }

    return cache_.insert( file, out ).value();

}

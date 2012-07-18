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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "File.h"
#include "PixmapEngine.h"
#include "XmlOptions.h"

#include <QtCore/QFileInfo>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionButton>

//__________________________________________________________
PixmapEngine& PixmapEngine::get( void )
{
    //! singleton
    static PixmapEngine singleton_;
    return singleton_;
}

//__________________________________________________________
PixmapEngine::PixmapEngine( void ):
    Counter( "PixmapEngine" )
{ Debug::Throw( "PixmapEngine::PixmapEngine.\n" ); }

//__________________________________________________________
bool PixmapEngine::reload( void )
{
    Debug::Throw( "PixmapEngine::reload.\n" );

    // load path from options
    QVector<QString> pathList( XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" ) );
    if( pathList == _pixmapPath() ) return false;

    _setPixmapPath( pathList );
    for( Cache::iterator iter = cache_.begin(); iter != cache_.end(); ++iter )
    { cache_[iter.key()] = _get( iter.key(), false ); }

    return true;
}

//__________________________________________________________
const QPixmap& PixmapEngine::_get( const QString& file, bool from_cache )
{
    Debug::Throw( "PixmapEngine::_get (file).\n" );

    // try find file in cache
    if( from_cache )
    {
        Cache::iterator iter( cache_.find( file ) );
        if( iter != cache_.end() ) return iter.value();
    }

    // create output
    QPixmap out;
    if( QFileInfo( file ).isAbsolute() ) { out = QPixmap( file ); }
    else {

        if( _pixmapPath().empty() ) _setPixmapPath( XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" ) );
        foreach( const QString& path, _pixmapPath() )
        {

            // skip empty path
            if( path.isEmpty() ) continue;

            // prepare filename
            File iconFile;

            // see if path is internal resource path
            if( path.left( 1 ) == ":" ) iconFile = File( file ).addPath( path );
            else iconFile = File( path ).find( file );

            // load pixmap
            if( !iconFile.isEmpty() )
            {
                out.load( iconFile );
                if( !out.isNull() ) break;
            }

        }

    }

    Cache::iterator iter = cache_.insert( file, out );
    return iter.value();

}

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

#include "SvgPlasmaInterface.h"
#include "SvgEngine.h"
#include "Svg.h"
#include "XmlOptions.h"

#include <QPainter>

namespace Svg
{

    //__________________________________________________________
    SvgEngine& SvgEngine::get( void )
    {
        static SvgEngine singleton;
        return singleton;
    }

    //__________________________________________________________
    SvgEngine::SvgEngine( void ):
        plasmaInterface_( 0 ),
        thread_( this )
    { connect( &thread_, SIGNAL(imageCacheAvailable(const Svg::ImageCache&)), this, SLOT(_processImageCache(const Svg::ImageCache&)) ); }

    //__________________________________________________________
    bool SvgEngine::reload( bool forced )
    {

        bool configurationChanged( svg_.updateConfiguration() );
        bool fileChanged( _loadSvg( forced ) );
        if( !isValid() )
        {
            cache_.clear();
            return true;
        }

        // reload cache
        if( configurationChanged || fileChanged )
        {

            SvgId::List svgIdList;
            for( auto&& iter = cache_.begin(); iter != cache_.end(); ++iter )
            { svgIdList << iter.key(); }

            cache_.clear();
            preload( svgIdList );

            // update margins and outer padding
            margins_ = svg_.margins();
            outerPadding_ = svg_.outerPadding();

            emit SvgEngine::changed();
            return true;
        }

        return false;

    }

    //__________________________________________________________
    void SvgEngine::preload( const SvgId::List& svg_ids )
    {

        if( svg_ids.empty() ) return;
        if( !isValid() ) return;
        if( thread_.isRunning() ) return;

        thread_.setSvgFile( svgFile_ );
        thread_.setSvgIdList( svg_ids );
        thread_.start();

    }

    //_______________________________________________
    void SvgEngine::_processImageCache( const ImageCache& cache )
    {

        for( auto&& iter = cache.begin(); iter != cache.end(); ++iter )
        { cache_.insert( iter.key(), QPixmap::fromImage( iter.value() ) ); }

    }

    //__________________________________________________________
    const QPixmap& SvgEngine::_get( const SvgId& id, bool fromCache )
    {

        PixmapCache::iterator iter( cache_.find( id ) );
        if( iter != cache_.end() ) return iter.value();

        // add to map
        QPixmap pixmap( id.size() );
        pixmap.fill( Qt::transparent );
        svg_.render( pixmap, id.id() );

        return cache_.insert( id, pixmap ).value();

    }

    //________________________________________________
    bool SvgEngine::_loadSvg( bool forced )
    {

        bool changed( false );

        // try get from plasma interface if needed
        #if !defined(Q_OS_WIN)
        if( XmlOptions::get().get<bool>( "SVG_USE_PLASMA_INTERFACE" ) )
        {

            bool first( !_hasPlasmaInterface() );
            if( first ) _initializePlasmaInterface();
            else if( forced ) changed |= _plasmaInterface().loadTheme();

            changed |= _plasmaInterface().setImagePath( (SvgPlasmaInterface::ImagePath) XmlOptions::get().get<int>( "SVG_PLASMA_IMAGE_PATH" ) );
            if( changed || first || forced )  _plasmaInterface().loadFile();

            if( _plasmaInterface().isValid() )
            {
                QString file( _plasmaInterface().fileName() );
                svg_.load( QString( file ) );
                if( svg_.isValid() )
                {

                    changed = ( svgFile_ != file );
                    svgFile_ = file;

                    return changed || forced;
                }
            }
        }
        #endif

        bool found( false );
        for( const auto& option:XmlOptions::get().specialOptions( "SVG_BACKGROUND" ) )
        {
            QString file( option.raw() );
            svg_.load( QString( file ) );
            if( svg_.isValid() )
            {
                changed = ( svgFile_ != file );
                svgFile_ = file;
                found = true;
                break;
            }
        }

        if( !found )  svg_.load( QString() );

        return changed || forced;

    }

    //________________________________
    void SvgEngine::_initializePlasmaInterface( void )
    {

        Q_ASSERT( !_hasPlasmaInterface() );
        plasmaInterface_ = new SvgPlasmaInterface( this );
        _plasmaInterface().loadTheme();
        connect( &_plasmaInterface(), SIGNAL(themeChanged()), SLOT(reload()) );

    }

};

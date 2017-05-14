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
        thread_( this )
    { connect( &thread_, SIGNAL(imageCacheAvailable(Svg::ImageCache)), this, SLOT(_processImageCache(Svg::ImageCache)) ); }

    //__________________________________________________________
    bool SvgEngine::needsReloadOnPaletteChange( void ) const
    { return ( renderer_.styleSheetIsUsed() && !(plasmaInterface_ && plasmaInterface_->hasThemePalette()) ); }

    //__________________________________________________________
    bool SvgEngine::reload( bool forced )
    {

        Debug::Throw(0) << "Svg::SvgEngine::reload - forced: " << forced << endl;

        bool configurationChanged( renderer_.updateConfiguration() );
        bool changed( _loadSvg( forced ) );

        if( !isValid() )
        {
            cache_.clear();
            return true;
        }

        // reload cache
        if( changed || configurationChanged )
        {

            SvgId::List svgIdList;
            for( auto&& iter = cache_.begin(); iter != cache_.end(); ++iter )
            { svgIdList << iter.key(); }

            cache_.clear();
            preload( svgIdList );

            // update margins and outer padding
            margins_ = renderer_.margins();
            outerPadding_ = renderer_.outerPadding();

            emit SvgEngine::changed();
            return true;
        }

        return false;

    }

    //__________________________________________________________
    void SvgEngine::preload( const SvgId::List& ids )
    {

        if( ids.empty() ) return;
        if( !isValid() ) return;
        if( thread_.isRunning() ) return;

        if( plasmaInterface_ && plasmaInterface_->hasThemePalette() )
        {

            auto palette( plasmaInterface_->themePalette() );
            thread_.createStyleSheet( palette );

        } else {

            QPalette palette;
            thread_.createStyleSheet( palette );

        }

        thread_.setSvgFile( svgFile_ );
        thread_.setSvgIdList( ids );
        thread_.start();

    }

    //_______________________________________________
    void SvgEngine::_processImageCache( const ImageCache& cache )
    {

        for( auto&& iter = cache.begin(); iter != cache.end(); ++iter )
        { cache_.insert( iter.key(), QPixmap::fromImage( iter.value() ) ); }

    }

    //__________________________________________________________
    QPixmap SvgEngine::_get( const SvgId& id, bool fromCache )
    {

        auto&& iter( cache_.find( id ) );
        if( iter != cache_.end() ) return iter.value();

        // add to map
        QPixmap pixmap( id.size() );
        pixmap.fill( Qt::transparent );
        renderer_.render( pixmap, id.id() );

        return cache_.insert( id, pixmap ).value();

    }

    //________________________________________________
    bool SvgEngine::_loadSvg( bool forced )
    {

        Debug::Throw(0) << "Svg::SvgEngine::_loadSvg - forced: " << forced << endl;
        bool changed( false );

        // try get from plasma interface if needed
        #if !defined(Q_OS_WIN)
        if( XmlOptions::get().get<bool>( "SVG_USE_PLASMA_INTERFACE" ) )
        {

            bool first( !plasmaInterface_ );
            if( first )
            {

                plasmaInterface_ = new SvgPlasmaInterface( this );
                connect( plasmaInterface_, SIGNAL(themeChanged()), SLOT(reload()) );

            }

            if( plasmaInterface_->loadTheme() ) changed = true;
            if( plasmaInterface_->hasThemePalette() )
            {

                renderer_.createStyleSheet( plasmaInterface_->themePalette() );

            } else {

                renderer_.createStyleSheet( QPalette() );

            }

            if( plasmaInterface_->setImagePath( (SvgPlasmaInterface::ImagePath) XmlOptions::get().get<int>( "SVG_PLASMA_IMAGE_PATH" ) ) ) changed = true;
            if( changed || first || forced ) plasmaInterface_->loadFile();

            if( plasmaInterface_->isValid() )
            {
                QString file( plasmaInterface_->fileName() );
                renderer_.load( QString( file ) );
                if( renderer_.isValid() )
                {

                    if( svgFile_ != file ) changed = true;
                    svgFile_ = file;

                    return changed || forced;
                }
            }
        }
        #endif

        // create style sheet
        renderer_.createStyleSheet( QPalette() );

        // find valid svg file
        bool found( false );
        for( const auto& option:XmlOptions::get().specialOptions( "SVG_BACKGROUND" ) )
        {

            QString file( option.raw() );
            renderer_.load( file );
            if( renderer_.isValid() )
            {
                changed = ( svgFile_ != file );
                svgFile_ = file;
                found = true;
                break;
            }
        }

        if( !found ) renderer_.load( QString() );

        return changed || forced;

    }

};

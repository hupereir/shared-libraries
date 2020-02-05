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

#include "SvgEngine.h"

#include "CppUtil.h"
#include "SvgPlasmaInterface.h"
#include "Svg.h"
#include "XmlOptions.h"

#include <QPainter>

namespace Svg
{

    //__________________________________________________________
    SvgEngine& SvgEngine::get()
    {
        static SvgEngine singleton;
        return singleton;
    }

    //__________________________________________________________
    SvgEngine::SvgEngine():
        thread_( this )
    { connect( &thread_, &SvgThread::imageCacheAvailable, this, &SvgEngine::_processImageCache ); }

    //__________________________________________________________
    bool SvgEngine::needsReloadOnPaletteChange() const
    { return ( renderer_.styleSheetIsUsed() && !(plasmaInterface_ && plasmaInterface_->hasThemePalette()) ); }

    //__________________________________________________________
    bool SvgEngine::hasThemePalette() const
    { return ( renderer_.styleSheetIsUsed() && plasmaInterface_ && plasmaInterface_->hasThemePalette() ); }

    //__________________________________________________________
    QPalette SvgEngine::themePalette() const
    { return hasThemePalette() ? plasmaInterface_->themePalette():QPalette(); }

    //__________________________________________________________
    bool SvgEngine::reload( bool forced )
    {

        Debug::Throw() << "Svg::SvgEngine::reload - forced: " << forced << endl;

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
            std::copy( cache_.keyBegin(), cache_.keyEnd(), std::back_inserter( svgIdList ) );

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
        {
            // insert only if not already in cache
            auto foundIter( cache_.lowerBound( iter.key() ) );
            if( foundIter == cache_.end() || !Base::areEquivalent( foundIter.key(), iter.key() ) )
            { Base::insert( cache_, foundIter, iter.key(), QPixmap::fromImage( iter.value() ) ); }
        }

    }

    //__________________________________________________________
    QPixmap SvgEngine::_get( const SvgId& id, bool fromCache )
    {

        auto iter( cache_.lowerBound( id ) );
        if( iter != cache_.end() && Base::areEquivalent( id, iter.key() ) ) return iter.value();
        else
        {
            // add to map
            QPixmap pixmap( id.size() );
            pixmap.fill( Qt::transparent );
            renderer_.render( pixmap, id.id() );

            return Base::insert(cache_, iter, id, pixmap ).value();
        }

    }

    //________________________________________________
    bool SvgEngine::_loadSvg( bool forced )
    {

        Debug::Throw() << "Svg::SvgEngine::_loadSvg - forced: " << forced << endl;
        bool changed( false );

        // try get from plasma interface if needed
        #if !defined(Q_OS_WIN)
        if( XmlOptions::get().get<bool>( QStringLiteral("SVG_USE_PLASMA_INTERFACE") ) )
        {

            bool first( !plasmaInterface_ );
            if( first )
            {

                plasmaInterface_ = new SvgPlasmaInterface( this );
                connect( plasmaInterface_, &SvgPlasmaInterface::themeChanged, this, QOverload<>::of( &SvgEngine::reload ) );

            }

            if( plasmaInterface_->loadTheme() ) changed = true;
            renderer_.createStyleSheet(
                plasmaInterface_->hasThemePalette() ?
                plasmaInterface_->themePalette():
                QPalette() );

            if( plasmaInterface_->setImagePath( (SvgPlasmaInterface::ImagePath) XmlOptions::get().get<int>( QStringLiteral("SVG_PLASMA_IMAGE_PATH") ) ) ) changed = true;
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
        for( const auto& option:XmlOptions::get().specialOptions( QStringLiteral("SVG_BACKGROUND") ) )
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

#ifndef SvgEngine_h
#define SvgEngine_h

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

#include "Margins.h"
#include "SvgRenderer.h"
#include "SvgThread.h"

#include <QSize>
#include <QObject>

//* svg namespace
namespace Svg
{

    class SvgPlasmaInterface;

    //* customized Icon factory to provide better looking disabled icons
    class SvgEngine: public QObject
    {

        Q_OBJECT;

        public:

        //* retrieve singleton
        static SvgEngine& get( void );

        //*@name accessors
        //@{

        //* create pixmap
        /** the size is stored into a cache to avoid duplicate pixmaps manipulations */
        static QPixmap get( SvgId id )
        { return get()._get( id ); }

        //* is valid
        bool isValid( void ) const
        { return renderer_.isValid(); }

        //* margins
        Base::Margins margins( void ) const
        { return margins_; }

        //* outer padding
        Base::Margins outerPadding( void ) const
        { return outerPadding_; }

        //* true if reload is needed on palette change
        bool needsReloadOnPaletteChange( void ) const;

        //@}

        //*@name modifiers
        //@{

        /** uses a separate thread, in order not to slow down application */
        void preload( const SvgId::List& );

        Q_SIGNALS:

        //* emitted when svg files are changed
        void changed( void );

        public Q_SLOTS:

        //* reload all icons set in cache from new path list
        /** return true if changed */
        bool forceReload( void )
        { return reload( true ); }

        //* reload all icons set in cache from new path list
        /** return true if changed */
        bool reload( bool forced = false );

        protected Q_SLOTS:

        //* process image cache generated from thread
        void _processImageCache( const Svg::ImageCache& );

        //* load svg
        bool _loadSvg( bool forced = false );

        private:

        //*@name non static methods are hidden
        //@{

        //* constructor
        SvgEngine( void );

        //* create icon
        /** the file is stored into a cache to avoid all pixmaps manipulations */
        QPixmap _get( const SvgId&, bool fromCache = true );

        //@}

        //* plasma interface
        SvgPlasmaInterface *plasmaInterface_ = nullptr;

        //* svg file
        QString svgFile_;

        //* svg renderer
        SvgRenderer renderer_;

        //* map size and pixmap
        PixmapCache cache_;

        //* thread preload sizes
        SvgThread thread_;

        //* margins
        Base::Margins margins_;

        //* outer padding
        Base::Margins outerPadding_;

    };
};

#endif

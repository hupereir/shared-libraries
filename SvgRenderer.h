#ifndef SvgRenderer_h
#define SvgRenderer_h

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

#include "Counter.h"
#include "Margins.h"

#include <QSvgRenderer>
#include <QPaintDevice>

//! construct pixmap of given size using Svg renderer
namespace SVG
{
    class SvgRenderer: public QSvgRenderer, public Counter
    {

        public:

        //! constructor
        SvgRenderer( void );

        //! destructor
        virtual ~SvgRenderer( void )
        {}

        //! render
        void render( QPaintDevice&, const QString& id = QString() );

        //! validity
        virtual bool isValid( void ) const
        { return isValid_; }

        //! configuration
        bool updateConfiguration( void );

        //! load file
        virtual bool load( const QString& );

        //! margins
        BASE::Margins margins( void ) const;

        //! outer padding
        BASE::Margins outerPadding( void ) const;

        // svg element enumeration
        enum SvgElement
        {
            TopLeft = 1<<0,
            Top = 1<<1,
            TopRight = 1<<2,
            Left = 1<<3,
            Center = 1<<4,
            Right = 1<<5,
            BottomLeft = 1<<6,
            Bottom = 1<<7,
            BottomRight = 1<<8,
            Ring = TopLeft|Top|TopRight|Left|Right|BottomLeft|Bottom|BottomRight,
            All = Ring|Center
        };

        Q_DECLARE_FLAGS( SvgElements, SvgElement );

        protected:

        //! true if svg has all elements matching prefix
        bool _hasPrefix( QString prefix = QString(), SvgElements = All ) const;

        //! true if svg has all margin elements matching prefix
        bool _hasMargins( QString prefix = QString(), SvgElements = (SvgElements)Top|Left|Right|Bottom ) const;

        //! render
        void _renderPanel( QPaintDevice& );

        //! render prefix to image
        /*! need to use images in order to be able to run in separate thread */
        void _render( QImage&, QString prefix = QString(), SvgElements = All, bool padding = true );

        private:

        //! true if overlay (when present) must be drawn
        bool drawOverlay_;

        //! validity
        bool isValid_;

        //! true if shadows are present
        bool hasShadow_;

        //! true if overlay is present
        bool hasOverlay_;

        //! overlay hints
        enum OverlayHint {
            OverlayNone = 0,
            OverlayStretch = 1<<0,
            OverlayTileHorizontal = 1<<1,
            OverlayTileVertical = 1<<2,
            OverlayPosRight = 1<<3,
            OverlayPosBottom = 1<<4
        };

        //! overlay hints
        int overlayHints_;

        //! painting hints
        enum Hint {
            HintNone,
            HintComposeOverBorder,
        };

        //! painting hints
        int hints_;

        //! prefix for loading mask
        QString maskPrefix_;

    };

};

Q_DECLARE_OPERATORS_FOR_FLAGS( SVG::SvgRenderer::SvgElements )

#endif

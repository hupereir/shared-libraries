#ifndef SvgRenderer_h
#define SvgRenderer_h

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

#include "base_svg_export.h"
#include "BaseSvgRenderer.h"
#include "Margins.h"

#include <QByteArray>
#include <QIODevice>
#include <QPaintDevice>
#include <QPalette>

//* construct pixmap of given size using Svg renderer
namespace Svg
{
    class BASE_SVG_EXPORT SvgRenderer: public BaseSvgRenderer
    {

        Q_OBJECT

        public:

        //* constructor
        explicit SvgRenderer()
        {}

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

        //*@name accessors
        //@{

        //* validity
        bool isValid() const
        { return isValid_; }

        //* margins
        Base::Margins margins() const;

        //* outer padding
        Base::Margins outerPadding() const;

        //@}

        //*@name modifiers
        //@{

        //* configuration
        bool updateConfiguration();

        //* load file
        bool load( const QString& ) override;

        //* render
        void render( QPaintDevice&, const QString& id = QString() );

        //@}

        protected:

        //* true if svg has all elements matching prefix
        bool _hasPrefix( QString prefix = QString(), SvgElements = All ) const;

        //* true if svg has all margin elements matching prefix
        bool _hasMargins( QString prefix = QString(), SvgElements = (SvgElements)Top|Left|Right|Bottom ) const;

        //* render
        void _renderPanel( QPaintDevice& );

        //* render prefix to image
        /** need to use images in order to be able to run in separate thread */
        void _render( QImage&, QString prefix = QString(), SvgElements = All, bool padding = true );

        private:

        //* true if overlay (when present) must be drawn
        bool drawOverlay_ = true;

        //* validity
        bool isValid_ = false;

        //* true if shadow margins are present
        bool hasShadowMargins_ = false;

        //* true if shadow prefix are present
        bool hasShadowPrefix_ = false;

        //* true if shadows are present
        bool hasShadow_ = false;

        //* true if overlay is present
        bool hasOverlay_ = false;

        //* overlay hints
        enum OverlayHint
        {
            OverlayNone = 0,
            OverlayStretch = 1<<0,
            OverlayTileHorizontal = 1<<1,
            OverlayTileVertical = 1<<2,
            OverlayPosRight = 1<<3,
            OverlayPosBottom = 1<<4
        };

        Q_DECLARE_FLAGS( OverlayHints, OverlayHint )

        //* overlay hints
        OverlayHints overlayHints_ = OverlayNone;

        //* painting hints
        enum class Hint
        {
            HintNone,
            HintComposeOverBorder,
        };

        //* painting hints
        Hint hints_ = Hint::HintNone;

        //* prefix for loading mask
        QString maskPrefix_;

    };

};

Q_DECLARE_OPERATORS_FOR_FLAGS( Svg::SvgRenderer::SvgElements )

#endif

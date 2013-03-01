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
        TRANSPARENCY::Margins margins( void ) const;

        //! shadows
        TRANSPARENCY::Margins shadows( void ) const;

        protected:

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

        //! true if svg has all elements matching prefix
        virtual bool _hasPrefix( QString prefix = "" ) const;

        //! render prefix to image
        virtual void _render( QImage& target, QString prefix = "", int elements = All, bool padding = true );

        private:

        //! true if overlay (when present) must be drawn
        bool drawOverlay_;

        //! validity
        bool isValid_;

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
#endif

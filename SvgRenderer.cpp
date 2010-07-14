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

/*!
\file SvgRenderer.cpp
\brief construct pixmap of given size using Svg renderer
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "SvgRenderer.h"
#include "Svg.h"
#include "XmlOptions.h"

#include <QtGui/QPainter>

namespace SVG
{

    //________________________________________________
    SvgRenderer::SvgRenderer( void ):
        QSvgRenderer(),
        drawOverlay_( true ),
        isValid_( false ),
        hasOverlay_( false ),
        overlayHints_( OverlayNone ),
        hints_( HintNone )
    {}

    //________________________________________________
    bool SvgRenderer::load( const QString& filename )
    {
      bool loaded( QSvgRenderer::load( filename ) );
      if( loaded )
      {
        isValid_ = _hasPrefix();
        hasOverlay_ = elementExists( SVG::Overlay );

        // centering hints
        overlayHints_ = OverlayNone;
        if( elementExists( "hint-overlay-pos-right" ) ) overlayHints_ |= OverlayPosRight;
        if( elementExists( "hint-overlay-pos-bottom" ) ) overlayHints_ |= OverlayPosBottom;

        if( elementExists( "hint-overlay-stretch") ) overlayHints_ |= OverlayStretch;
        else if( elementExists( "hint-overlay-tile-horizontal" ) ) overlayHints_ |= OverlayTileHorizontal;
        else if( elementExists( "hint-overlay-tile-vertical") ) overlayHints_ |= OverlayTileVertical;

        hints_ = HintNone;
        if( elementExists( "hint-compose-over-border" ) ) hints_ |= HintComposeOverBorder;

        if( _hasPrefix( "mask-overlay" ) ) maskPrefix_ = "mask-overlay";
        else if( _hasPrefix( "mask" ) ) maskPrefix_ = "mask";
        else maskPrefix_ = QString();

      } else {

        isValid_ = false;

      }

      return loaded;

    }

    //________________________________________________
    bool SvgRenderer::updateConfiguration( void )
    {
        bool drawOverlay( XmlOptions::get().get<bool>( "SVG_DRAW_OVERLAY" ) );
        if( drawOverlay == drawOverlay_ ) return false;
        drawOverlay_ = drawOverlay;
        return true;
    }

    //________________________________________________
    void SvgRenderer::render( QPaintDevice& device, double offset, const QString& id )
    {

        // check device size
        if( device.height() <= 0 || device.width() <= 0 ) return;

        // create painter
        if( !id.isEmpty() )
        {

            double width = device.width() + 2.0*offset;
            double height = device.height() + 2.0*offset;

            QPainter painter( &device );
            QSvgRenderer::render( &painter, id, QRectF( QPointF( - offset, - offset ), QSizeF( width, height ) ) );

        } else {

            // create painter
            QPainter painter( &device );

            // size
            QSize size( device.width(), device.height() );

            //Overlays
            if( drawOverlay_ && hasOverlay_ )
            {

                // compute painting rect
                QSizeF overlaySize = boundsOnElement(SVG::Overlay).size();
                Margins margins( _margins() );

                // size hints
                if( overlayHints_ & OverlayStretch ) overlaySize = size;
                else if( overlayHints_ & OverlayTileHorizontal ) overlaySize.setWidth( size.width() );
                else if( overlayHints_ & OverlayTileVertical ) overlaySize.setHeight( size.height() );
                QRectF overlayRect( QPointF(0,0), overlaySize );

                // create image and paint
                QImage overlayImage( QSize( device.width(), device.height() ),  QImage::Format_ARGB32 );

                overlayImage.fill( Qt::transparent );
                QPainter overlayPainter( &overlayImage );

                // centering hints
                if( overlayHints_ & OverlayPosRight ) overlayPainter.translate( device.width() - overlayRect.width(), 0 );
                if( overlayHints_ & OverlayPosBottom ) overlayPainter.translate( 0, device.height() - overlayRect.height() );

                QSvgRenderer::render( &overlayPainter, SVG::Overlay, overlayRect );
                overlayPainter.end();

                // draw on main painter
                painter.drawImage( QPointF(0,0), overlayImage );

            }

            // center
            if( hints_ & HintComposeOverBorder )
            {
                QImage centerImage( size, QImage::Format_ARGB32 );
                centerImage.fill( Qt::transparent );
                _render( centerImage, offset, "", Center, false );
                painter.drawImage( QPoint(0,0), centerImage );
            }

            // set mask
            if( !maskPrefix_.isEmpty() && ((hints_ & HintComposeOverBorder) || (drawOverlay_ && hasOverlay_ )) )
            {
                // create mask
                QImage maskImage( size, QImage::Format_ARGB32 );
                maskImage.fill( Qt::transparent );
                _render( maskImage, offset, maskPrefix_ );

                painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                painter.drawImage( QPointF(0,0), maskImage );

                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            }

            // main image
            QImage mainImage( size, QImage::Format_ARGB32 );
            mainImage.fill( Qt::transparent );

            //
            if( hints_ & HintComposeOverBorder ) _render( mainImage, offset, "", Ring );
            else  _render( mainImage, offset );

            painter.drawImage( QPoint(0,0), mainImage );

        }

        return;

    }

    //________________________________________________
    bool SvgRenderer::_hasPrefix( QString prefix ) const
    {

        if( !prefix.isEmpty() ) prefix+="-";

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( !elementExists( prefix+SVG::TopLeft ) ) return false;
        if( !elementExists( prefix+SVG::Top ) ) return false;
        if( !elementExists( prefix+SVG::TopRight ) ) return false;

        if( !elementExists( prefix+SVG::BottomLeft ) ) return false;
        if( !elementExists( prefix+SVG::Bottom ) ) return false;
        if( !elementExists( prefix+SVG::BottomRight ) ) return false;

        if( !elementExists( prefix+SVG::Left ) ) return false;
        if( !elementExists( prefix+SVG::Center ) ) return false;
        if( !elementExists( prefix+SVG::Right ) ) return false;
        return true;

    }

    //________________________________________________
    SvgRenderer::Margins SvgRenderer::_margins( void ) const
    {
        Margins out;
        if( elementExists( SVG::MarginLeft ) ) out.left = boundsOnElement( SVG::MarginLeft ).size().width();
        if( elementExists( SVG::MarginRight ) ) out.right = boundsOnElement( SVG::MarginRight ).size().width();
        if( elementExists( SVG::MarginTop ) ) out.top = boundsOnElement( SVG::MarginTop ).size().height();
        if( elementExists( SVG::MarginBottom ) ) out.bottom = boundsOnElement( SVG::MarginBottom ).size().height();

        return out;
    }

    //________________________________________________
    void SvgRenderer::_render( QImage& target, double offset, QString prefix, int elements, bool padding  )
    {

        if( !prefix.isEmpty() ) prefix += "-";

        QSizeF cornerSize( padding ? boundsOnElement( prefix+SVG::TopLeft ).size(): QSize(0,0) );
        double width = target.width() + 2.0*(offset - cornerSize.width() );
        double height = target.height() + 2.0*( offset - cornerSize.height() );

        // paint svg onto image
        QPainter painter( &target );

        // center
        if( elements & Center ) QSvgRenderer::render( &painter, prefix+SVG::Center, QRectF( QPointF( cornerSize.width() - offset, cornerSize.height() - offset ), QSizeF( width, height ) ) );

        // corners
        if( elements & TopLeft ) QSvgRenderer::render( &painter, prefix+SVG::TopLeft, QRectF( QPointF( -offset, -offset ), cornerSize ) );
        if( elements & TopRight ) QSvgRenderer::render( &painter, prefix+SVG::TopRight, QRectF( QPointF( width + cornerSize.width() - offset , -offset ), cornerSize ) );
        if( elements & BottomLeft ) QSvgRenderer::render( &painter, prefix+SVG::BottomLeft, QRectF( QPointF( -offset, height+cornerSize.height() - offset ), cornerSize ) );
        if( elements & BottomRight ) QSvgRenderer::render( &painter, prefix+SVG::BottomRight, QRectF( QPointF( width + cornerSize.width() - offset, height+cornerSize.height() - offset ), cornerSize ) );

        // sides
        if( elements & Top ) QSvgRenderer::render( &painter, prefix+SVG::Top, QRectF( QPointF( cornerSize.width()-offset, -offset ), QSizeF( width, cornerSize.height() ) ) );
        if( elements & Bottom ) QSvgRenderer::render( &painter, prefix+SVG::Bottom, QRectF( QPointF( cornerSize.width() - offset, height+cornerSize.height() - offset ), QSizeF( width, cornerSize.height() ) ) );
        if( elements & Left ) QSvgRenderer::render( &painter, prefix+SVG::Left, QRectF( QPointF( -offset, cornerSize.height() - offset ), QSizeF( cornerSize.width(), height ) ) );
        if( elements & Right ) QSvgRenderer::render( &painter, prefix+SVG::Right, QRectF( QPointF( width + cornerSize.width()- offset, cornerSize.height()- offset ), QSizeF( cornerSize.width(), height ) ) );
        painter.end();
        return;
    }

}

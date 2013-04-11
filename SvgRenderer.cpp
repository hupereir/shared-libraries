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

#include "SvgRenderer.h"
#include "Svg.h"
#include "XmlOptions.h"

#include <QPainter>

namespace SVG
{

    //________________________________________________
    SvgRenderer::SvgRenderer( void ):
        QSvgRenderer(),
        Counter( "SVG::SvgRendered" ),
        drawOverlay_( true ),
        isValid_( false ),
        hasShadow_( false ),
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
        hasShadow_ = _hasPrefix( "shadow", Ring ) && _hasMargins( "shadow" );
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
    void SvgRenderer::render( QPaintDevice& device, const QString& id )
    {

        // check device size
        if( device.height() <= 0 || device.width() <= 0 ) return;

        // create painter
        if( !id.isEmpty() )
        {

            double width = device.width();
            double height = device.height();

            QPainter painter( &device );
            QSvgRenderer::render( &painter, id, QRectF( QPointF( 0, 0 ), QSizeF( width, height ) ) );

        } else {


            if( hasShadow_ )
            {

                // create painter
                QPainter painter( &device );

                // shadow image
                QSize size( device.width(), device.height() );
                QImage shadowImage( size, QImage::Format_ARGB32_Premultiplied );
                shadowImage.fill( Qt::transparent );

                _render( shadowImage, "shadow", Ring );

                painter.drawImage( QPoint(0,0), shadowImage );

                // main image
                const BASE::Margins outerPadding( this->outerPadding() );
                size.rwidth() -= outerPadding.width();
                size.rheight() -= outerPadding.height();
                QImage mainImage( size, QImage::Format_ARGB32_Premultiplied );
                mainImage.fill( Qt::transparent );

                _renderPanel( mainImage );
                painter.drawImage( outerPadding.topLeft(), mainImage );

            } else _renderPanel( device );

        }

        return;

    }

    //________________________________________________
    bool SvgRenderer::_hasPrefix( QString prefix, SvgElements mask ) const
    {

        if( !prefix.isEmpty() ) prefix+="-";

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( (mask & TopLeft) && !elementExists( prefix+SVG::TopLeft ) ) return false;
        if( (mask & Top) && !elementExists( prefix+SVG::Top ) ) return false;
        if( (mask & TopRight) && !elementExists( prefix+SVG::TopRight ) ) return false;

        if( (mask & BottomLeft) && !elementExists( prefix+SVG::BottomLeft ) ) return false;
        if( (mask & Bottom) && !elementExists( prefix+SVG::Bottom ) ) return false;
        if( (mask & BottomRight) && !elementExists( prefix+SVG::BottomRight ) ) return false;

        if( (mask & Left) && !elementExists( prefix+SVG::Left ) ) return false;
        if( (mask & Center) && !elementExists( prefix+SVG::Center ) ) return false;
        if( (mask & Right) && !elementExists( prefix+SVG::Right ) ) return false;
        return true;

    }


    //________________________________________________
    bool SvgRenderer::_hasMargins( QString prefix, SvgElements mask ) const
    {

        if( !prefix.isEmpty() ) prefix+="-";

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( (mask & Top) && !elementExists( prefix+SVG::MarginTop ) ) return false;
        if( (mask & Left) && !elementExists( prefix+SVG::MarginLeft ) ) return false;
        if( (mask & Right) && !elementExists( prefix+SVG::MarginRight ) ) return false;
        if( (mask & Bottom) && !elementExists( prefix+SVG::MarginBottom ) ) return false;
        return true;

    }

    //________________________________________________
    BASE::Margins SvgRenderer::margins( void ) const
    {
        BASE::Margins out;
        if( elementExists( SVG::MarginLeft ) ) out.setLeft( boundsOnElement( SVG::MarginLeft ).width() );
        if( elementExists( SVG::MarginRight ) ) out.setRight( boundsOnElement( SVG::MarginRight ).width() );
        if( elementExists( SVG::MarginTop ) ) out.setTop( boundsOnElement( SVG::MarginTop ).height() );
        if( elementExists( SVG::MarginBottom ) ) out.setBottom( boundsOnElement( SVG::MarginBottom ).height() );

        // add shadow size if available
        if( hasShadow_ ) out += outerPadding();

        return out;
    }

    //________________________________________________
    BASE::Margins SvgRenderer::outerPadding( void ) const
    {
        BASE::Margins out;
        if( hasShadow_ )
        {

            out.setLeft( boundsOnElement( "shadow-"+SVG::MarginLeft ).width() );
            out.setRight( boundsOnElement( "shadow-"+SVG::MarginRight ).width() );
            out.setTop( boundsOnElement( "shadow-"+SVG::MarginTop ).height() );
            out.setBottom( boundsOnElement( "shadow-"+SVG::MarginBottom ).height() );

        } else {

            if( elementExists( SVG::ShadowLeft ) ) out.setLeft( boundsOnElement( SVG::ShadowLeft ).size().width() );
            if( elementExists( SVG::ShadowRight ) ) out.setRight( boundsOnElement( SVG::ShadowRight ).size().width() );
            if( elementExists( SVG::ShadowTop ) ) out.setTop( boundsOnElement( SVG::ShadowTop ).size().height() );
            if( elementExists( SVG::ShadowBottom ) ) out.setBottom( boundsOnElement( SVG::ShadowBottom ).size().height() );

        }

        return out;

    }

    //________________________________________________
    void SvgRenderer::_renderPanel( QPaintDevice& device )
    {

        // create painter
        QPainter painter( &device );

        // size
        QSize size( device.width(), device.height() );

        //Overlays
        if( drawOverlay_ && hasOverlay_ )
        {

            // compute painting rect
            QSizeF overlaySize = boundsOnElement(SVG::Overlay).size();

            // size hints
            if( overlayHints_ & OverlayStretch ) overlaySize = size;
            else if( overlayHints_ & OverlayTileHorizontal ) overlaySize.setWidth( size.width() );
            else if( overlayHints_ & OverlayTileVertical ) overlaySize.setHeight( size.height() );
            QRectF overlayRect( QPointF(0,0), overlaySize );

            // create image and paint
            QImage overlayImage( QSize( device.width(), device.height() ),  QImage::Format_ARGB32_Premultiplied );

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
            QImage centerImage( size, QImage::Format_ARGB32_Premultiplied );
            centerImage.fill( Qt::transparent );
            _render( centerImage, "", Center, false );
            painter.drawImage( QPoint(0,0), centerImage );
        }

        // set mask
        if( !maskPrefix_.isEmpty() && ((hints_ & HintComposeOverBorder) || (drawOverlay_ && hasOverlay_ )) )
        {
            // create mask
            QImage maskImage( size, QImage::Format_ARGB32_Premultiplied );
            maskImage.fill( Qt::transparent );
            _render( maskImage, maskPrefix_ );

            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            painter.drawImage( QPointF(0,0), maskImage );

            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        }

        // main image
        QImage mainImage( size, QImage::Format_ARGB32_Premultiplied );
        mainImage.fill( Qt::transparent );

        //
        if( hints_ & HintComposeOverBorder ) _render( mainImage, "", Ring );
        else  _render( mainImage );

        painter.drawImage( QPoint(0,0), mainImage );

    }

    //________________________________________________
    void SvgRenderer::_render( QImage& target, QString prefix, SvgElements elements, bool padding  )
    {

        // check target
        if( target.isNull() ) return;

        const QRect targetRect( target.rect() );
        if( !targetRect.isValid() ) return;

        // check prefix
        if( !prefix.isEmpty() ) prefix += "-";

        // paint svg onto image
        QPainter painter( &target );

        QRectF centerRect( targetRect );
        if( padding )
        {
            centerRect.adjust(
                boundsOnElement( prefix+SVG::Left ).width(),
                boundsOnElement( prefix+SVG::Top ).height(),
                -boundsOnElement( prefix+SVG::Right ).width(),
                -boundsOnElement( prefix+SVG::Bottom ).height() );
        }

        // center
        if( centerRect.isValid() && elements & Center )
        { QSvgRenderer::render( &painter, prefix+SVG::Center, centerRect ); }

        if( padding )
        {

            if( elements & TopLeft )
            {
                // topLeft corner
                painter.setClipRect( QRect( targetRect.topLeft(), targetRect.center() ) );
                QSvgRenderer::render( &painter, prefix+SVG::TopLeft, QRectF( QPointF( 0, 0 ), boundsOnElement( prefix+SVG::TopLeft ).size() ) );
            }

            if( elements & TopRight )
            {
                // topRight corner
                painter.setClipRect( QRect( QPoint(  targetRect.center().x()+1, targetRect.top() ), QPoint( targetRect.right(), targetRect.center().y() ) ) );
                QSvgRenderer::render( &painter, prefix+SVG::TopRight, QRectF( QPointF( centerRect.right(), 0 ), boundsOnElement( prefix+SVG::TopRight ).size() ) );
            }

            if( elements & BottomLeft )
            {

                // topRight corner
                painter.setClipRect( QRect( QPoint(  targetRect.left(), targetRect.center().y()+1 ), QPoint( targetRect.center().x(), targetRect.bottom() ) ) );
                QSvgRenderer::render( &painter, prefix+SVG::BottomLeft, QRectF( QPointF( 0, centerRect.bottom() ), boundsOnElement( prefix+SVG::BottomRight ).size() ) );
            }

            if( elements & BottomRight )
            {
                // bottomRight corner
                painter.setClipRect( QRect( targetRect.center()+QPoint( 1, 1 ), targetRect.bottomRight() ) );
                QSvgRenderer::render( &painter, prefix+SVG::BottomRight, QRectF( centerRect.bottomRight(), boundsOnElement( prefix+SVG::BottomRight ).size() ) );
            }

            if( centerRect.width() > 0 )
            {

                if( elements & Top )
                {
                    // top size
                    painter.setClipRect( QRect( targetRect.topLeft(), QPoint( targetRect.right(), targetRect.center().y() ) ) );
                    QSvgRenderer::render( &painter, prefix+SVG::Top, QRectF( QPointF( centerRect.left(), 0 ), QSizeF( centerRect.width(), boundsOnElement( prefix+SVG::Top ).height() ) ) );
                }

                if( elements & Bottom )
                {
                    // bottom size
                    painter.setClipRect( QRect( QPoint( targetRect.left(), targetRect.center().y()+1 ), targetRect.bottomRight() ) );
                    QSvgRenderer::render( &painter, prefix+SVG::Bottom, QRectF( centerRect.bottomLeft(), QSizeF( centerRect.width(), boundsOnElement( prefix+SVG::Bottom ).height() ) ) );
                }

            }

            if( centerRect.height() > 0 )
            {

                if( elements & Left )
                {
                    // left side
                    painter.setClipRect( QRect( targetRect.topLeft(), QPoint( targetRect.center().x(), targetRect.bottom() ) ) );
                    QSvgRenderer::render( &painter, prefix+SVG::Left, QRectF( QPointF( 0, centerRect.top() ), QSizeF( boundsOnElement( prefix+SVG::Left ).width(), centerRect.height() ) ) );
                }

                if( elements & Right )
                {
                    painter.setClipRect( QRect( QPoint( targetRect.center().x()+1, targetRect.top() ), targetRect.bottomRight() ) );
                    QSvgRenderer::render( &painter, prefix+SVG::Right, QRectF( centerRect.topRight(), QSizeF( boundsOnElement( prefix+SVG::Right ).width(), centerRect.height() ) ) );
                }

            }

        }

        painter.end();
        return;
    }

}

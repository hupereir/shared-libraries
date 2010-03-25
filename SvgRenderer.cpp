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
        drawOverlay_( true )
    {}

    //________________________________________________
    bool SvgRenderer::updateConfiguration( void )
    {
        bool drawOverlay( XmlOptions::get().get<bool>( "SVG_DRAW_OVERLAY" ) );
        if( drawOverlay == drawOverlay_ ) return false;
        drawOverlay_ = drawOverlay;
        return true;
    }

    //________________________________________________
    void SvgRenderer::render( QPaintDevice& device, const double& offset, const QString& id )
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

            // one need to use QImage all over the place here
            // to make sure that SvgRenderer can work in non-gui thread
            QImage mainImage(
                QSize(device.width(), device.height() ),
                QImage::Format_ARGB32 );
            mainImage.fill( Qt::transparent );

            QSizeF cornerSize( boundsOnElement( SVG::TOP_LEFT ).size() );
            double width = device.width() + 2.0*(offset - cornerSize.width() );
            double height = device.height() + 2.0*( offset - cornerSize.height() );

            // paint svg onto image
            QPainter mainPainter( &mainImage );
            QSvgRenderer::render( &mainPainter, SVG::TOP_LEFT, QRectF( QPointF( -offset, -offset ), cornerSize ) );
            QSvgRenderer::render( &mainPainter, SVG::TOP, QRectF( QPointF( cornerSize.width()-offset, -offset ), QSizeF( width, cornerSize.height() ) ) );
            QSvgRenderer::render( &mainPainter, SVG::TOP_RIGHT, QRectF( QPointF( width + cornerSize.width() - offset , -offset ), cornerSize ) );

            QSvgRenderer::render( &mainPainter, SVG::BOTTOM_LEFT, QRectF( QPointF( -offset, height+cornerSize.height() - offset ), cornerSize ) );
            QSvgRenderer::render( &mainPainter, SVG::BOTTOM, QRectF( QPointF( cornerSize.width() - offset, height+cornerSize.height() - offset ), QSizeF( width, cornerSize.height() ) ) );
            QSvgRenderer::render( &mainPainter, SVG::BOTTOM_RIGHT, QRectF( QPointF( width + cornerSize.width() - offset, height+cornerSize.height() - offset ), cornerSize ) );

            QSvgRenderer::render( &mainPainter, SVG::LEFT, QRectF( QPointF( -offset, cornerSize.height() - offset ), QSizeF( cornerSize.width(), height ) ) );
            QSvgRenderer::render( &mainPainter, SVG::RIGHT, QRectF( QPointF( width + cornerSize.width()- offset, cornerSize.height()- offset ), QSizeF( cornerSize.width(), height ) ) );
            QSvgRenderer::render( &mainPainter, SVG::CENTER, QRectF( QPointF( cornerSize.width() - offset, cornerSize.height() - offset ), QSizeF( width, height ) ) );
            mainPainter.end();

            QPainter painter( &device );
            painter.drawImage( QPoint(0,0), mainImage );

            //Overlays
            if( drawOverlay_ && _hasOverlay() )
            {

                // compute painting rect
                QSizeF overlaySize = boundsOnElement(SVG::OVERLAY).size();
                Margins margins( _margins() );

                // size hints
                if( elementExists( "hint-overlay-stretch") ) overlaySize = QSizeF( width, height );
                else if( elementExists( "hint-overlay-tile-horizontal" ) ) overlaySize.setWidth(width );
                else if( elementExists( "hint-overlay-tile-vertical") ) overlaySize.setHeight( height );

                QRectF overlayRect( QPointF(0,0), overlaySize );

                // create image and paint
                QImage overlayImage(
                    QSize( device.width(), device.height() ),
                    QImage::Format_ARGB32 );

                overlayImage.fill( Qt::transparent );
                QPainter overlayPainter( &overlayImage );

                // centering hints
                if( elementExists( "hint-overlay-pos-right" ) ) overlayPainter.translate( device.width() - overlayRect.width(), 0 );
                if( elementExists( "hint-overlay-pos-bottom" ) ) overlayPainter.translate( 0, device.height() - overlayRect.height() );

                QSvgRenderer::render( &overlayPainter, SVG::OVERLAY, overlayRect );
                overlayPainter.end();

                if( _hasOverlayMask() )
                {

                    // create mask
                    QImage maskImage(
                        QSize(device.width(), device.height() ),
                        QImage::Format_ARGB32 );
                    maskImage.fill( Qt::transparent );

                    QPainter maskPainter( &maskImage );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_TOP_LEFT, QRectF( QPointF( -offset, -offset ), cornerSize ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_TOP, QRectF( QPointF( cornerSize.width()-offset, -offset ), QSizeF( width, cornerSize.height() ) ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_TOP_RIGHT, QRectF( QPointF( width + cornerSize.width() - offset , -offset ), cornerSize ) );

                    QSvgRenderer::render( &maskPainter, SVG::MASK_BOTTOM_LEFT, QRectF( QPointF( -offset, height+cornerSize.height() - offset ), cornerSize ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_BOTTOM, QRectF( QPointF( cornerSize.width() - offset, height+cornerSize.height() - offset ), QSizeF( width, cornerSize.height() ) ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_BOTTOM_RIGHT, QRectF( QPointF( width + cornerSize.width() - offset, height+cornerSize.height() - offset ), cornerSize ) );

                    QSvgRenderer::render( &maskPainter, SVG::MASK_LEFT, QRectF( QPointF( -offset, cornerSize.height() - offset ), QSizeF( cornerSize.width(), height ) ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_RIGHT, QRectF( QPointF( width + cornerSize.width()- offset, cornerSize.height()- offset ), QSizeF( cornerSize.width(), height ) ) );
                    QSvgRenderer::render( &maskPainter, SVG::MASK_CENTER, QRectF( QPointF( cornerSize.width() - offset, cornerSize.height() - offset ), QSizeF( width, height ) ) );
                    maskPainter.end();

                    // update mask
                    overlayImage.setAlphaChannel( maskImage.alphaChannel() );

                }

                // draw on main painter
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                painter.drawImage(
                    QPointF(0,0),
                    overlayImage );

            }

        }

        return;

    }

    //________________________________________________
    bool SvgRenderer::isValid( void ) const
    {

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( !elementExists( SVG::TOP_LEFT ) ) return false;
        if( !elementExists( SVG::TOP ) ) return false;
        if( !elementExists( SVG::TOP_RIGHT ) ) return false;

        if( !elementExists( SVG::BOTTOM_LEFT ) ) return false;
        if( !elementExists( SVG::BOTTOM ) ) return false;
        if( !elementExists( SVG::BOTTOM_RIGHT ) ) return false;

        if( !elementExists( SVG::LEFT ) ) return false;
        if( !elementExists( SVG::CENTER ) ) return false;
        if( !elementExists( SVG::RIGHT ) ) return false;
        return true;

    }

    //________________________________________________
    bool SvgRenderer::_hasOverlay( void ) const
    { return elementExists( SVG::OVERLAY ); }

    //________________________________________________
    bool SvgRenderer::_hasOverlayMask( void ) const
    {

        // check base class
        if( !QSvgRenderer::isValid() ) return false;

        // make sure needed elements are present
        if( !elementExists( SVG::MASK_TOP_LEFT ) ) return false;
        if( !elementExists( SVG::MASK_TOP ) ) return false;
        if( !elementExists( SVG::MASK_TOP_RIGHT ) ) return false;

        if( !elementExists( SVG::MASK_BOTTOM_LEFT ) ) return false;
        if( !elementExists( SVG::MASK_BOTTOM ) ) return false;
        if( !elementExists( SVG::MASK_BOTTOM_RIGHT ) ) return false;

        if( !elementExists( SVG::MASK_LEFT ) ) return false;
        if( !elementExists( SVG::MASK_CENTER ) ) return false;
        if( !elementExists( SVG::MASK_RIGHT ) ) return false;
        return true;

    }

    //________________________________________________
    SvgRenderer::Margins SvgRenderer::_margins( void ) const
    {
        Margins out;
        if( elementExists( SVG::MARGIN_LEFT ) ) out.left = boundsOnElement( SVG::MARGIN_LEFT ).size().width();
        if( elementExists( SVG::MARGIN_RIGHT ) ) out.right = boundsOnElement( SVG::MARGIN_RIGHT ).size().width();
        if( elementExists( SVG::MARGIN_TOP ) ) out.top = boundsOnElement( SVG::MARGIN_TOP ).size().height();
        if( elementExists( SVG::MARGIN_BOTTOM ) ) out.bottom = boundsOnElement( SVG::MARGIN_BOTTOM ).size().height();

        return out;
    }
};

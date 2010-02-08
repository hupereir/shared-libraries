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
      QImage main_image(
        QSize(device.width(), device.height() ),
        QImage::Format_ARGB32 );
      main_image.fill( Qt::transparent );

      QSizeF corner_size( boundsOnElement( SVG::TOP_LEFT ).size() );
      double width = device.width() + 2.0*(offset - corner_size.width() );
      double height = device.height() + 2.0*( offset - corner_size.height() );

      // paint svg onto image
      QPainter main_painter( &main_image );
      QSvgRenderer::render( &main_painter, SVG::TOP_LEFT, QRectF( QPointF( -offset, -offset ), corner_size ) );
      QSvgRenderer::render( &main_painter, SVG::TOP, QRectF( QPointF( corner_size.width()-offset, -offset ), QSizeF( width, corner_size.height() ) ) );
      QSvgRenderer::render( &main_painter, SVG::TOP_RIGHT, QRectF( QPointF( width + corner_size.width() - offset , -offset ), corner_size ) );

      QSvgRenderer::render( &main_painter, SVG::BOTTOM_LEFT, QRectF( QPointF( -offset, height+corner_size.height() - offset ), corner_size ) );
      QSvgRenderer::render( &main_painter, SVG::BOTTOM, QRectF( QPointF( corner_size.width() - offset, height+corner_size.height() - offset ), QSizeF( width, corner_size.height() ) ) );
      QSvgRenderer::render( &main_painter, SVG::BOTTOM_RIGHT, QRectF( QPointF( width + corner_size.width() - offset, height+corner_size.height() - offset ), corner_size ) );

      QSvgRenderer::render( &main_painter, SVG::LEFT, QRectF( QPointF( -offset, corner_size.height() - offset ), QSizeF( corner_size.width(), height ) ) );
      QSvgRenderer::render( &main_painter, SVG::RIGHT, QRectF( QPointF( width + corner_size.width()- offset, corner_size.height()- offset ), QSizeF( corner_size.width(), height ) ) );
      QSvgRenderer::render( &main_painter, SVG::CENTER, QRectF( QPointF( corner_size.width() - offset, corner_size.height() - offset ), QSizeF( width, height ) ) );
      main_painter.end();

      QPainter painter( &device );
      painter.drawImage( QPoint(0,0), main_image );

      //Overlays
      if( drawOverlay_ && elementExists("overlay") )
      {


        QPointF pos = QPoint(0, 0);
        QSizeF overlay_size = boundsOnElement("overlay").size();

        if( elementExists( "hint-overlay-stretch") ) overlay_size = QSizeF( width, height );
        else if( elementExists( "hint-overlay-tile-horizontal" ) ) overlay_size.setWidth(width );
        else if( elementExists( "hint-overlay-tile-vertical") ) overlay_size.setHeight( height );

        QImage overlay_image(
          QSize( device.width(), device.height() ),
          QImage::Format_ARGB32 );

        overlay_image.fill( Qt::transparent );

        QPainter overlay_painter( &overlay_image );
        QSvgRenderer::render( &overlay_painter, "overlay", QRectF( pos, overlay_size ) );
        overlay_painter.end();

        // update mask
        overlay_image.setAlphaChannel( main_image.alphaChannel() );

        // draw on main painter
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage( pos, overlay_image );

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

};

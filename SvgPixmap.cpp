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
  \file SvgPixmap.cpp
  \brief construct pixmap of given size using Svg renderer
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>
#include "Svg.h"
#include "SvgPixmap.h"

namespace SVG
{
  //________________________________________________
  SvgPixmap::SvgPixmap( const QSize& size ):
    QPixmap( size )
  { };
  
  //________________________________________________
  SvgPixmap& SvgPixmap::render( QSvgRenderer& svg, const double& offset )
  {
    
    if( size().isEmpty() ) return *this;
    
    fill( Qt::transparent );
    QPainter svg_painter( this );
    
    QSizeF corner_size( svg.boundsOnElement( SVG::TOP_LEFT ).size() );    
    double width = SvgPixmap::width() + 2.0*(offset - corner_size.width() );
    double height = SvgPixmap::height() + 2.0*( offset - corner_size.height() );
    
    // paint corners
    svg.render( &svg_painter, SVG::TOP_LEFT, QRectF( QPointF( -offset, -offset ), corner_size ) ); 
    svg.render( &svg_painter, SVG::TOP, QRectF( QPointF( corner_size.width()-offset, -offset ), QSizeF( width, corner_size.height() ) ) );
    svg.render( &svg_painter, SVG::TOP_RIGHT, QRectF( QPointF( width + corner_size.width() - offset , -offset ), corner_size ) ); 
    
    svg.render( &svg_painter, SVG::BOTTOM_LEFT, QRectF( QPointF( -offset, height+corner_size.height() - offset ), corner_size ) ); 
    svg.render( &svg_painter, SVG::BOTTOM, QRectF( QPointF( corner_size.width() - offset, height+corner_size.height() - offset ), QSizeF( width, corner_size.height() ) ) );
    svg.render( &svg_painter, SVG::BOTTOM_RIGHT, QRectF( QPointF( width + corner_size.width() - offset, height+corner_size.height() - offset ), corner_size ) ); 
    
    svg.render( &svg_painter, SVG::LEFT, QRectF( QPointF( -offset, corner_size.height() - offset ), QSizeF( corner_size.width(), height ) ) );
    svg.render( &svg_painter, SVG::RIGHT, QRectF( QPointF( width + corner_size.width()- offset, corner_size.height()- offset ), QSizeF( corner_size.width(), height ) ) );
    svg.render( &svg_painter, SVG::CENTER, QRectF( QPointF( corner_size.width() - offset, corner_size.height() - offset ), QSizeF( width, height ) ) );
    return *this;
    
  }
  
};

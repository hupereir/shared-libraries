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

#include <QPainter>
#include "Svg.h"
#include "SvgRenderer.h"

namespace SVG
{
  
  //________________________________________________
  void SvgRenderer::render( QPaintDevice& device, const double& offset )
  {
    
    // check device size
    if( device.height() <= 0 || device.width() <= 0 ) return;
    
    // create painter
    QPainter painter( &device );    
    QSizeF corner_size( boundsOnElement( SVG::TOP_LEFT ).size() );    
    double width = device.width() + 2.0*(offset - corner_size.width() );
    double height = device.height() + 2.0*( offset - corner_size.height() );
    
    // paint corners
    QSvgRenderer::render( &painter, SVG::TOP_LEFT, QRectF( QPointF( -offset, -offset ), corner_size ) ); 
    QSvgRenderer::render( &painter, SVG::TOP, QRectF( QPointF( corner_size.width()-offset, -offset ), QSizeF( width, corner_size.height() ) ) );
    QSvgRenderer::render( &painter, SVG::TOP_RIGHT, QRectF( QPointF( width + corner_size.width() - offset , -offset ), corner_size ) ); 
    
    QSvgRenderer::render( &painter, SVG::BOTTOM_LEFT, QRectF( QPointF( -offset, height+corner_size.height() - offset ), corner_size ) ); 
    QSvgRenderer::render( &painter, SVG::BOTTOM, QRectF( QPointF( corner_size.width() - offset, height+corner_size.height() - offset ), QSizeF( width, corner_size.height() ) ) );
    QSvgRenderer::render( &painter, SVG::BOTTOM_RIGHT, QRectF( QPointF( width + corner_size.width() - offset, height+corner_size.height() - offset ), corner_size ) ); 
    
    QSvgRenderer::render( &painter, SVG::LEFT, QRectF( QPointF( -offset, corner_size.height() - offset ), QSizeF( corner_size.width(), height ) ) );
    QSvgRenderer::render( &painter, SVG::RIGHT, QRectF( QPointF( width + corner_size.width()- offset, corner_size.height()- offset ), QSizeF( corner_size.width(), height ) ) );
    QSvgRenderer::render( &painter, SVG::CENTER, QRectF( QPointF( corner_size.width() - offset, corner_size.height() - offset ), QSizeF( width, height ) ) );

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

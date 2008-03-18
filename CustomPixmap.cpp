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
  \file CustomPixmap.cpp
  \brief customized QPixmap to look for icon of given name in list of directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QBitmap>
#include <QImage>
#include <QPainter>

#include "CustomPixmap.h"
#include "Debug.h"
#include "File.h"

using namespace std;

//_________________________________________________
CustomPixmap CustomPixmap::find( 
  const string& file, 
  const std::list<string>& path, 
  bool case_sensitive )
{
  Debug::Throw( "CustomPixmap::find.\n" );
  for( list<string>::const_iterator iter = path.begin(); iter != path.end(); iter++ )
  {
    
    // prepare filename
    File icon_file;
    
    // skip empty path
    if( iter->empty() ) continue;
    
    // see if path is internal resource path
    if( iter->substr( 0, 1 ) == ":" ) icon_file = File( file ).addPath( *iter );
    else icon_file = File( *iter ).find( file, case_sensitive );
    
    // load pixmap
    if( !icon_file.empty() )
    {
      load( icon_file.c_str() );
      if( !isNull() ) break;
    }
  }
    
  return *this;
    
} 

//_________________________________________________
CustomPixmap CustomPixmap::tint( const QColor& base_color, const double& intensity ) const
{ 
  if( isNull() ) return *this;
  
  QImage image( toImage() );
  int width( image.width() );
  int height( image.height() );

  double merged_red = intensity*base_color.red();
  double merged_green = intensity*base_color.green();
  double merged_blue = intensity*base_color.blue();
  double no_intensity( 1.0-intensity );
  QColor merged_color; 
  for( int x = 0; x < width; x++ )
  {
    for( int y = 0; y < height; y++ ) 
    {
      
      QColor color( image.pixel( x, y ) );    
      merged_color.setRgb( qRgb( 
        (unsigned int)( no_intensity*color.red() + merged_red ), 
        (unsigned int)( no_intensity*color.green() + merged_green ), 
        (unsigned int)( no_intensity*color.blue() + merged_blue ) ) );
      
      image.setPixel( x, y, merged_color.rgb() );
    }
  }
  
  CustomPixmap out( image );
  out.setAlphaChannel( alphaChannel() );
  return out;

}

//_________________________________________________
CustomPixmap CustomPixmap::merge( const QPixmap& pixmap, Corner corner ) const
{ 
  if( isNull() ) return *this;
  
  QImage source_image( toImage() );
  QPainter painter( &source_image );
  switch( corner )
  {
    
    case TOP_RIGHT:
    painter.drawPixmap( QPoint( width()-pixmap.width(), 0 ), pixmap );
    break;
    
    case BOTTOM_LEFT:
    painter.drawPixmap( QPoint( 0, height()-pixmap.height() ), pixmap );
    break;
    
    case BOTTOM_RIGHT:
    painter.drawPixmap( QPoint( width()-pixmap.width(), height()-pixmap.height() ), pixmap );
    break;
    
    case CENTER:
    painter.drawPixmap(  QPoint( (width()-pixmap.width())/2, (height()-pixmap.height())/2 ), pixmap );
    break;
    
    default:
    case TOP_LEFT:
    painter.drawPixmap( QPoint( 0, 0 ), pixmap );
    break;
    
  }
    
  painter.end();
  return CustomPixmap().fromImage( source_image ); 
}

//_________________________________________________
CustomPixmap CustomPixmap::scale( const unsigned int& width, const unsigned int& height ) const
{
  QImage image( toImage() );
  return CustomPixmap().fromImage( image.scaled( width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation) );
}

//_________________________________________________
CustomPixmap CustomPixmap::empty( const QSize& size, const QColor& color, const bool& transparent ) const
{
  
  Debug::Throw( "CustomPixmap::empty.\n" );
  CustomPixmap out( size );
  out.fill( color );
  if( transparent ) 
  {
    QBitmap mask( size );
    mask.clear();
    out.setMask( mask );
  }
  
  return out;

}

//_________________________________________________
CustomPixmap CustomPixmap::disabled( void )
{
  
  Debug::Throw( "CustomPixmap::disabled.\n" );
  QImage image( toImage() );
  
  // retrieve dimensions
  int width( image.width() );
  int height( image.height() );
  
  QColor merged_color;
  for( int x = 0; x < width; x++ )
  {
    for( int y = 0; y < height; y++ ) 
    {
      QColor color( image.pixel( x, y ) );
      int gray( 128 + qGray( color.rgb() )/2 );
      merged_color.setRgb( gray, gray, gray );
      
      image.setPixel( x, y, merged_color.rgb() );
    }
  }

  CustomPixmap out( image );
  out.setAlphaChannel( alphaChannel() );
  return out;
  
}

//_________________________________________________
CustomPixmap CustomPixmap::active( void )
{
  
  Debug::Throw( "CustomPixmap::active.\n" );
  QImage image( toImage() );
  
  // retrieve dimensions
  int width( image.width() );
  int height( image.height() );
  
  QColor merged_color;
  for( int x = 0; x < width; x++ )
  {
    for( int y = 0; y < height; y++ ) 
    {

      QColor color( image.pixel( x, y ) );
      color = color.light( 120 );
      image.setPixel( x, y, color.rgb() );
      
    }
  }

  CustomPixmap out( image );
  out.setAlphaChannel( alphaChannel() );
  return out;
  
}

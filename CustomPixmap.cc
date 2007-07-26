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
  \file CustomPixmap.cc
  \brief customized QPixmap to look for icon of given name in list of directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QBitmap>
#include <QImage>

#include "CustomPixmap.h"
#include "Debug.h"
#include "File.h"

using namespace std;

//_________________________________________________
CustomPixmap CustomPixmap::find( 
  const std::string& file, 
  const std::list<string>& path, 
  bool case_sensitive )
{
  Debug::Throw( "CustomPixmap::find.\n" );
  bool found( false );
  for( list<string>::const_iterator iter = path.begin(); iter != path.end(); iter++ )
  {
    Debug::Throw() << "CustomPixmap::find - searching " << *iter << endl;
    File icon_file( File( *iter ).find( file, case_sensitive ) );
    if( icon_file != File::EMPTY_STRING )
    {
      load( icon_file.c_str() );
      found = true;
      break;
    }
  }
  
  if( !found )
  {
    Debug::Throw(0) << "CustomPixmap::find - cannot find " << file << endl;
    for( list<string>::const_iterator iter = path.begin(); iter != path.end(); iter++ )
    Debug::Throw(0) << "CustomPixmap::find - searching " << *iter << endl;
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
      merged_color.setRgb( qRgba( 
        (unsigned int)( no_intensity*color.red() + merged_red ), 
        (unsigned int)( no_intensity*color.green() + merged_green ), 
        (unsigned int)( no_intensity*color.blue() + merged_blue ), 
        qAlpha( color.rgb() ) ) );
      
      image.setPixel( x, y, merged_color.rgb() );
    }
  }
  
  return CustomPixmap().fromImage( image ); 

}

//_________________________________________________
CustomPixmap CustomPixmap::merge( const QPixmap& pixmap ) const
{ 
  if( isNull() ) return *this;
  
  QImage source_image( toImage() );
  int source_width( source_image.width() );
  int source_height( source_image.height() );

  QImage merge_image( pixmap.toImage() );
  int merge_width( merge_image.width() );
  int merge_height( merge_image.height() );
  
  for( int x = 0; x < min( source_width, merge_width ); x++ )
  {
    for( int y = 0; y < min( source_height, merge_height ); y++ ) 
    {
  
      QColor source_color( source_image.pixel( x, y ) );    
      QColor merge_color( merge_image.pixel( x, y ) );
      
      int source_alpha( qAlpha( source_color.rgb() ) );
      int merge_alpha( qAlpha( merge_color.rgb() ) );
      double intensity = double( source_alpha )/double( source_alpha + merge_alpha );
      double no_intensity = 1-intensity;
      
      QColor new_color;
      new_color.setRgb( qRgba( 
        (unsigned int)( intensity*source_color.red() + no_intensity*merge_color.red() ), 
        (unsigned int)( intensity*source_color.green() + no_intensity*merge_color.green() ), 
        (unsigned int)( intensity*source_color.blue() + no_intensity*merge_color.blue() ), 
        max( source_alpha, merge_alpha ) ) );
      
      source_image.setPixel( x, y, new_color.rgb() );
    }
  }
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
      merged_color.setAlpha( color.alpha() );
      
      image.setPixel( x, y, merged_color.rgb() );
    }
  }

  return CustomPixmap( image );
  
}

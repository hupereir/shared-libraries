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
  \file PixmapEngine.cpp
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QCheckBox>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

#include "CustomPixmap.h"
#include "PixmapEngine.h"
#include "XmlOptions.h"

using namespace std;

//__________________________________________________________
PixmapEngine* PixmapEngine::singleton_(0);

//__________________________________________________________
PixmapEngine::PixmapEngine( void ):
  Counter( "PixmapEngine" )
{ Debug::Throw( "PixmapEngine::PixmapEngine.\n" ); }

//__________________________________________________________
bool PixmapEngine::reload( void )
{ 
  Debug::Throw( "PixmapEngine::reload.\n" );
 
  // load path from options
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  if( path_list == _pixmapPath() ) return false;

  _setPixmapPath( path_list );
  for( Cache::iterator iter = cache_.begin(); iter != cache_.end(); iter++ )
  { cache_[iter->first] = _get( iter->first ); }

  return true;
}

//__________________________________________________________
QPixmap PixmapEngine::_get( const string& file )
{
  Debug::Throw( "PixmapEngine::_get (file).\n" );
  
  if( _pixmapPath().empty() ) _setPixmapPath( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );

  // try find file in cache
  Cache::iterator iter( cache_.find( file ) );
  if( iter != cache_.end() ) return iter->second;
  
  // create output
  QPixmap out;
  for( list<string>::const_iterator iter = _pixmapPath().begin(); iter != _pixmapPath().end(); iter++ )
  {
    
    // skip empty path
    if( iter->empty() ) continue;

    // prepare filename
    File icon_file;
    
    // see if path is internal resource path
    if( iter->substr( 0, 1 ) == ":" ) icon_file = File( file ).addPath( *iter );
    else icon_file = File( *iter ).find( file );
    
    // load pixmap
    if( !icon_file.empty() )
    {
      out.load( icon_file.c_str() );
      if( !out.isNull() ) break;
    }
  }
    
  return out;
  
}

//__________________________________________________________
void PixmapEngine::_createCheckPixmap( void )
{
  //ebug::Throw( 0, "PixmapEngine::_createCheckPixmap.\n" );
  cout << "PixmapEngine::_createCheckPixmap.\n";
  QCheckBox local;
  local.setChecked( true );
  
  QStyle *style( local.style() );
  QStyleOptionButton opt;
  opt.initFrom( &local );
  int width( style->pixelMetric( QStyle::PM_IndicatorWidth ) );
  check_pixmap_ = QPixmap( width, width );
  check_pixmap_.fill( Qt::transparent );
  
  QPainter painter( &check_pixmap_ );
  style->drawPrimitive( QStyle::PE_IndicatorCheckBox, &opt, &painter, &local);

}

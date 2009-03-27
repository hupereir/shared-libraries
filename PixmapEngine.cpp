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

#include <QFileInfo>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

#include "File.h"
#include "PixmapEngine.h"
#include "XmlOptions.h"

using namespace std;

//__________________________________________________________
PixmapEngine& PixmapEngine::get( void )
{ 
  //! singleton
  static PixmapEngine singleton_;
  return singleton_; 
}

//__________________________________________________________
PixmapEngine::PixmapEngine( void ):
  Counter( "PixmapEngine" )
{ Debug::Throw( "PixmapEngine::PixmapEngine.\n" ); }

//__________________________________________________________
bool PixmapEngine::reload( void )
{ 
  Debug::Throw( "PixmapEngine::reload.\n" );
 
  // load path from options
  list<QString> path_list( XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" ) );
  if( path_list == _pixmapPath() ) return false;

  _setPixmapPath( path_list );
  for( Cache::iterator iter = cache_.begin(); iter != cache_.end(); iter++ )
  { cache_[iter->first] = _get( iter->first, false ); }

  return true;
}

//__________________________________________________________
const QPixmap& PixmapEngine::_get( const QString& file, bool from_cache )
{
  Debug::Throw( "PixmapEngine::_get (file).\n" );
  
  // try find file in cache
  if( from_cache )
  {
    Cache::iterator iter( cache_.find( file ) );
    if( iter != cache_.end() ) return iter->second;
  }
  
  // create output
  QPixmap out;
  if( QFileInfo( file ).isAbsolute() ) { out = QPixmap( file ); } 
  else {
    
    if( _pixmapPath().empty() ) _setPixmapPath( XmlOptions::get().specialOptions<QString>( "PIXMAP_PATH" ) );
    for( list<QString>::const_iterator iter = _pixmapPath().begin(); iter != _pixmapPath().end(); iter++ )
    {
    
      // skip empty path
      if( iter->isEmpty() ) continue;
      
      // prepare filename
      File icon_file;
      
      // see if path is internal resource path
      if( iter->left( 1 ) == ":" ) icon_file = File( file ).addPath( *iter );
      else icon_file = File( *iter ).find( file );
      
      // load pixmap
      if( !icon_file.isEmpty() )
      {
        out.load( icon_file );
        if( !out.isNull() ) break;
      }
    
    }
    
  }
    
  return cache_.insert( make_pair( file, out ) ).first->second;
  
}

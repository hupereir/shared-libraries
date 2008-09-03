 
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
  \file IconEngine.cpp
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomPixmap.h"
#include "PixmapEngine.h"
#include "IconEngine.h"

using namespace std;

//__________________________________________________________
IconEngine IconEngine::singleton_;

//__________________________________________________________
IconEngine::IconEngine( void ):
  Counter( "IconEngine" )
{ Debug::Throw( "IconEngine::IconEngine.\n" ); }

//__________________________________________________________
bool IconEngine::reload( void )
{ 
  Debug::Throw( "IconEngine::reload.\n" );
  
  if( !PixmapEngine::get().reload() ) return false;
  for( Cache::iterator iter = cache_.begin(); iter != cache_.end(); iter++ )
  { cache_[iter->first] = _get( iter->first ); }
  
  return true;
}

//__________________________________________________________
QIcon IconEngine::_get( const string& file )
{
  Debug::Throw( "IconEngine::_get (file).\n" );

  // try find file in cache
  Cache::iterator iter( cache_.find( file ) );
  if( iter != cache_.end() ) return iter->second;
  
  QIcon out( _get( PixmapEngine::get( file ) ) );
  cache_.insert( make_pair( file, out ) );
  return out;
  
}

//__________________________________________________________
QIcon IconEngine::_get( const QPixmap& pixmap )
{
  
  Debug::Throw( "IconEngine::get (QPixmap).\n" );
  if( pixmap.isNull() ) return QIcon( pixmap );
  
  QIcon out( pixmap );
  //out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::On );
  //out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::Off );
  return out;
  
}

//__________________________________________________________
QIcon IconEngine::_get( const QIcon& icon )
{
  
  Debug::Throw( "IconEngine::get (QIcon).\n" );
  
  QIcon out( icon );
  //QPixmap pixmap;
  //if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::On )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::On );
  //if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::Off )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::Off );
  return out;
  
}

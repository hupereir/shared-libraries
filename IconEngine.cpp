
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
IconEngine& IconEngine::get( void )
{
  //! singleton
  static IconEngine singleton_;
  return singleton_;
}

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
  { cache_[iter->first] = _get( iter->first, false ); }

  return true;
}

//__________________________________________________________
const QIcon& IconEngine::_get( const QString& file, bool from_cache )
{
  Debug::Throw( "IconEngine::_get (file).\n" );

  // try find file in cache
  if( from_cache )
  {
    Cache::iterator iter( cache_.find( file ) );
    if( iter != cache_.end() ) return iter->second;
  }

  QIcon out( _get( PixmapEngine::get( file, from_cache ) ) );
  return cache_.insert( make_pair( file, out ) ).first->second;

}

//__________________________________________________________
QIcon IconEngine::_get( const QPixmap& pixmap )
{

  Debug::Throw( "IconEngine::get (QPixmap).\n" );
  if( pixmap.isNull() ) return QIcon( pixmap );

  QIcon out( pixmap );

#if QT_VERSION < 0x040300
  // better looking disabled icons are generated only for versions prior to qt 4.3
  QPixmap disabled( CustomPixmap( pixmap ).disabled() );
  out.addPixmap( disabled, QIcon::Disabled, QIcon::On );
  out.addPixmap( disabled, QIcon::Disabled, QIcon::Off );
#endif

//   // better looking disabled icons are generated only for versions prior to qt 4.3
//   QPixmap active( CustomPixmap( pixmap ).active() );
//   out.addPixmap( active, QIcon::Active, QIcon::On );
//   out.addPixmap( active, QIcon::Active, QIcon::Off );

  return out;

}

//__________________________________________________________
QIcon IconEngine::_get( const QIcon& icon )
{

  Debug::Throw( "IconEngine::get (QIcon).\n" );

  QIcon out( icon );

#if QT_VERSION < 0x040300
  // better looking disabled icons are generated only for versions prior to qt 4.3
  QPixmap pixmap;
  if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::On )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::On );
  if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::Off )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::Off );
#endif

  return out;

}

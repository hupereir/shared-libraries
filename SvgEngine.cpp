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
  \file SvgEngine.cpp
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPainter>

#include "SvgPlasmaInterface.h"
#include "SvgEngine.h"
#include "Svg.h"
#include "XmlOptions.h"

using namespace std;

namespace SVG
{

  //__________________________________________________________
  SvgEngine& SvgEngine::get( void )
  {
    static SvgEngine singleton;
    return singleton;
  }

  //__________________________________________________________
  SvgEngine::SvgEngine( void ):
    Counter( "SvgEngine" ),
    plasma_interface_( 0 ),
    svg_offset_( 0 ),
    thread_( this )
  {
    Debug::Throw( "SvgEngine::SvgEngine.\n" );

    XmlOptions::get().setAutoDefault( true );
    XmlOptions::get().keep( "SVG_BACKGROUND" );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background.svg", Option::RECORDABLE|Option::CURRENT ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-white.svg" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-simple.svg" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-translucent.svg" ) );
    XmlOptions::get().set( "USE_SVG", Option("1") );
    XmlOptions::get().set( "SVG_OFFSET", Option("0") );
    XmlOptions::get().set( "SVG_USE_PLASMA_INTERFACE", Option( "1" ) );
    XmlOptions::get().set( "SVG_PLASMA_IMAGE_PATH", Option( "dialogs/background" ) );
    XmlOptions::get().setAutoDefault( false );

    return;
  }

  //__________________________________________________________
  SvgEngine::~SvgEngine( void )
  { Debug::Throw( "SvgEngine::~SvgEngine.\n" ); }


  //__________________________________________________________
  bool SvgEngine::reload( const bool& forced )
  {
    Debug::Throw( "SvgEngine::reload.\n" );

    // try load background file
    bool changed( _loadSvg( forced ) );
    if( !isValid() )
    {
      cache_.clear();
      return true;
    }

    // check offset
    int svg_offset( XmlOptions::get().get<int>( "SVG_OFFSET" ) );

    // reload cache
    if( changed || svg_offset != svg_offset_ )
    {

      svg_offset_ = svg_offset;
      SvgId::List svg_id_list;
      for( SVG::PixmapCache::iterator iter = cache_.begin(); iter != cache_.end(); iter++ )
      { svg_id_list.push_back( iter->first ); }

      cache_.clear();
      preload( svg_id_list );

      emit SvgEngine::changed();
      return true;
    }

    return false;

  }

  //__________________________________________________________
  void SvgEngine::preload( const SvgId::List& svg_ids )
  {

    Debug::Throw( "SvgEngine::preload.\n" );
    if( svg_ids.empty() ) return;
    if( !isValid() ) return;
    if( thread_.isRunning() ) return;

    thread_.setSvgFile( svg_file_ );
    thread_.setSvgOffset( svg_offset_ );
    thread_.setSvgIdList( svg_ids );
    thread_.start();

  }

  //_______________________________________________
  void SvgEngine::customEvent( QEvent* event )
  {

    Debug::Throw( "SvgEngine::customEvent.\n" );

    if( event->type() != SvgEvent::eventType() ) return QObject::customEvent( event );

    SvgEvent* svg_event( dynamic_cast<SvgEvent*>(event) );
    if( !svg_event ) return QObject::customEvent( event );

    for( ImageCache::const_iterator iter = svg_event->cache().begin(); iter != svg_event->cache().end(); iter++ )
    { cache_.insert( make_pair( iter->first, QPixmap::fromImage( iter->second ) ) ); }

  }

  //__________________________________________________________
  const QPixmap& SvgEngine::_get( const SvgId& id, bool from_cache )
  {
    Debug::Throw( "SvgEngine::_get.\n" );

    PixmapCache::iterator iter( cache_.find( id ) );
    if( iter != cache_.end() ) return iter->second;

    // add to map
    QPixmap pixmap( id.size() );
    pixmap.fill( Qt::transparent );
    svg_.render( pixmap, svg_offset_, id.id() );

    return cache_.insert( make_pair( id, pixmap ) ).first->second;

  }

  //________________________________________________
  bool SvgEngine::_loadSvg( const bool& forced )
  {

    Debug::Throw( "SvgEngine::_loadSvg.\n" );

    bool changed( false );

    // try get from plasma interface if needed
    #ifndef Q_WS_WIN
    if( XmlOptions::get().get<bool>( "SVG_USE_PLASMA_INTERFACE" ) )
    {

      bool first( !_hasPlasmaInterface() );
      if( first ) _initializePlasmaInterface();
      else if( forced ) changed |= _plasmaInterface().loadTheme();

      changed |= _plasmaInterface().setImagePath( XmlOptions::get().raw( "SVG_PLASMA_IMAGE_PATH" ) );
      if( changed || first || forced )  _plasmaInterface().loadFile();

      if( _plasmaInterface().isValid() )
      {
        QString file( _plasmaInterface().fileName() );
        svg_.load( QString( file ) );
        if( svg_.isValid() )
        {

          changed = ( svg_file_ != file );
          svg_file_ = file;

          return changed || forced;
        }
      }
    }
    #endif

    bool found( false );
    Options::List file_list( XmlOptions::get().specialOptions( "SVG_BACKGROUND" ) );
    for( Options::List::const_iterator iter = file_list.begin(); iter != file_list.end(); iter++ )
    {
      QString file( iter->raw() );
      svg_.load( QString( file ) );
      if( svg_.isValid() )
      {
        changed = ( svg_file_ != file );
        svg_file_ = file;
        found = true;
        break;
      }
    }

    if( !found )  svg_.load( QString() );
    return changed || forced;

  }

  //________________________________
  void SvgEngine::_initializePlasmaInterface( void )
  {

    Debug::Throw( "SvgEngine::_initializePlasmaInterface.\n" );
    assert( !_hasPlasmaInterface() );
    plasma_interface_ = new SvgPlasmaInterface( this );
    _plasmaInterface().loadTheme();
    connect( &_plasmaInterface(), SIGNAL( themeChanged() ), SLOT( reload() ) );

  }

};

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
    svg_offset_( 0 ),
    thread_( this )
  {
    Debug::Throw( "SvgEngine::SvgEngine.\n" );
    XmlOptions::get().setAutoDefault( true );
    XmlOptions::get().keep( "SVG_BACKGROUND" );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background.svg", Option::RECORDABLE|Option::CURRENT ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-simple.svg" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-translucent.svg" ) );
    XmlOptions::get().add( "SVG_BACKGROUND", Option( ":/svg/background-translucent-2.svg" ) );
    XmlOptions::get().set( "USE_SVG", Option("1") );
    XmlOptions::get().set( "SVG_OFFSET", Option("0") );
    XmlOptions::get().setAutoDefault( false );
    return;
  }
  
  //__________________________________________________________
  bool SvgEngine::reload( void )
  { 
    Debug::Throw( "SvgEngine::reload.\n" );
    
    // try load background file
    bool changed( _loadSvg() );
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
      vector<QSize> sizes;
      for( SvgPixmap::Cache::iterator iter = cache_.begin(); iter != cache_.end(); iter++ )
      { sizes.push_back( iter->first ); }

      cache_.clear();
      preload( sizes );
      
      return true;
    }
    
    return false;
    
  }
  
  //__________________________________________________________
  void SvgEngine::preload( vector<QSize> sizes )
  { 
    
    Debug::Throw( "SvgEngine::preload.\n" );
    if( sizes.empty() ) return;
    if( !isValid() ) return;
    if( thread_.isRunning() ) return;
    
    thread_.setSvgFile( svg_file_ );
    thread_.setSvgOffset( svg_offset_ );
    thread_.setSizes( sizes );
    thread_.start();
    
  }
  
  //_______________________________________________ 
  void SvgEngine::customEvent( QEvent* event )
  {
    
    Debug::Throw( "SvgEngine::customEvent.\n" );
    
    if( event->type() != SvgEvent::eventType() ) return QObject::customEvent( event );
    
    SvgEvent* svg_event( dynamic_cast<SvgEvent*>(event) );
    if( !svg_event ) return QObject::customEvent( event );

    for( SvgImage::Cache::const_iterator iter = svg_event->cache().begin(); iter != svg_event->cache().end(); iter++ )
    { cache_.insert( make_pair( iter->first, QPixmap::fromImage( iter->second ) ) ); }    
    
  }  
  
  //__________________________________________________________
  const QPixmap& SvgEngine::_get( const QSize& size, bool from_cache )
  {
    Debug::Throw( "SvgEngine::_get.\n" );
    
    // try find file in cache
    if( from_cache )
    {
      SvgPixmap::Cache::iterator iter( cache_.find( size ) );
      if( iter != cache_.end() ) return iter->second;
    }
    
    // add to map
    SvgPixmap pixmap( size );
    pixmap.render( svg_, svg_offset_ );
    return cache_.insert( make_pair( size, pixmap ) ).first->second;
    
  }
  
  //________________________________________________
  bool SvgEngine::_loadSvg( void )
  {
    
    Debug::Throw( "SvgEngine::_loadSvg.\n" );
    
    // get options
    bool changed( true );
    bool found( false );
    Options::List file_list( XmlOptions::get().specialOptions( "SVG_BACKGROUND" ) );
    for( Options::List::const_iterator iter = file_list.begin(); iter != file_list.end(); iter++ )
    {
      QString file( iter->raw() );
      svg_.load( QString( file ) );
      if( svg_.isValid() && _validateSvg() ) 
      {
        changed = ( svg_file_ != file );
        svg_file_ = file;
        found = true; 
        break;
      }
    }
    
    if( !found )  svg_.load( QString() );
    return changed;
    
  }
  
  //________________________________________________
  bool SvgEngine::_validateSvg( void ) const
  {
    Debug::Throw( "SvgEngine::_validateSvg.\n" );
    
    if( !svg_.elementExists( SVG::TOP_LEFT ) ) return false;
    if( !svg_.elementExists( SVG::TOP ) ) return false;
    if( !svg_.elementExists( SVG::TOP_RIGHT ) ) return false;
    
    if( !svg_.elementExists( SVG::BOTTOM_LEFT ) ) return false;
    if( !svg_.elementExists( SVG::BOTTOM ) ) return false;
    if( !svg_.elementExists( SVG::BOTTOM_RIGHT ) ) return false;
    
    if( !svg_.elementExists( SVG::LEFT ) ) return false;
    if( !svg_.elementExists( SVG::CENTER ) ) return false;
    if( !svg_.elementExists( SVG::RIGHT ) ) return false;
    return true;
  }
  
}

#ifndef PixmapEngine_h
#define PixmapEngine_h

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
  \file PixmapEngine.h
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPixmap>
#include <map>
#include <list>
#include <assert.h>

#include "Counter.h"
#include "Debug.h"

//! customized Icon factory to provide better looking disabled icons
class PixmapEngine: public Counter
{
  
  public:
  
  //! retrieve singleton
  static PixmapEngine& get( void )
  { 
    if( !singleton_ ) singleton_ = new PixmapEngine();
    return *singleton_; 
  }
  
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  static QPixmap get( const std::string& file )
  { return get()._get( file ); }

  //! check pixmap
  QPixmap getCheckPixmap( void ) 
  {
    if( check_pixmap_.isNull() ) _createCheckPixmap();
    return check_pixmap_;
  }
  
  //! map files and QPixmap
  typedef std::map< std::string, QPixmap > Cache;

  //! map files and QPixmap
  class Pair: public std::pair<std::string, QPixmap >, public Counter
  {
    
    public:
    
    //! constructor
    Pair( void ):
      Counter( "PixmapEngine::Pair" )
    {}
    
    //! constructor
    Pair( const std::pair<std::string, QPixmap >& pair ):
      std::pair<std::string, QPixmap >( pair ),
      Counter( "PixmapEngine::Pair" )
    {}
    
    //! equal to operator
    bool operator == ( const Pair& pair ) const
    { return first == pair.first; }
    
    //! less than operator
    bool operator < ( const Pair& pair ) const
    { return first < pair.first; }
    
  };
  
  //! return cache
  static const Cache& cache( void )
  { return get().cache_; }
  
  //! reload all icons set in cache from new path list
  bool reload( void );
  
  protected:
    
  //! pixmap path
  void _setPixmapPath( const std::list< std::string >& path_list )
  { pixmap_path_ = path_list; }
  
  //! pixmap path
  const std::list< std::string >& _pixmapPath( void ) const
  { return pixmap_path_; }
  
  private:
      
  //! singleton
  static PixmapEngine* singleton_;
    
  //!@name non static methods are hidden
  //@{
  
  //! constructor
  PixmapEngine( void );
  
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  QPixmap _get( const std::string& file );

  //! create check pixmap
  void _createCheckPixmap( void );
  
  //@}
  
  //! pixmap path
  std::list< std::string > pixmap_path_;

  //! map files and QPixmap
  Cache cache_;
  
  //! this is a "checked" pixmap, build using QStyle, similar to checked check-boxes
  QPixmap check_pixmap_;
    
};

#endif

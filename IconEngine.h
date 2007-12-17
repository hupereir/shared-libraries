#ifndef IconEngine_h
#define IconEngine_h

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
  \file IconEngine.h
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QIcon>
#include <map>
#include <list>
#include <assert.h>

#include "Counter.h"
#include "Debug.h"

//! customized Icon factory to provide better looking disabled icons
class IconEngine: public Counter
{
  
  public:
  
  //! retrieve singleton
  static IconEngine& get( void )
  { return singleton_; }
  
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  static QIcon get( const std::string& file, const std::list<std::string> path_list )
  { return get()._get( file, path_list ); }

  /*! get icon matching file, from chache */
  static QIcon get( const std::string& file )
  { 
    assert( get().use_cache_ );
    return get().cache_[file];
  }

  //! create icon
  static QIcon get( const QPixmap& pixmap )
  { return get()._get( pixmap ); }
  
  //! create icon
  static QIcon get( const QIcon& icon )
  { return get()._get( icon ); }
  
  //! reload all icons set in cache from new path list
  void reload(const std::list<std::string> path_list  )
  { 
    assert( get().use_cache_ );
    for( Cache::iterator iter = get().cache_.begin(); iter != get().cache_.end(); iter++ )
    { get().cache_[iter->first] = get()._get( iter->first, path_list ); }
  }
  
  //! reload cache
  void clear( void )
  { cache_.clear(); }

  //! set the use_cache flag
  void setUseCache( const bool& value )
  { use_cache_ = value; }
  
  private:
    
  //! constructor is private to force use of singleton
  IconEngine( void ):
    Counter( "IconEngine" ),
    use_cache_( true )
  { Debug::Throw( "IconEngine::IconEngine.\n" ); } 
  
  //! singleton
  static IconEngine singleton_;
  
  //!@name non static methods are hidden
  //@{
  
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  QIcon _get( const std::string& file, const std::list<std::string> path_list );

  //! create icon
  QIcon _get( const QPixmap& pixmap );
  
  //! create icon
  QIcon _get( const QIcon& icon );

  //@}
  
  //! use cache to store icons generated from filenames
  bool use_cache_;
  
  //! map files and QIcon
  typedef std::map< std::string, QIcon > Cache;
  
  //! map files and QIcon
  Cache cache_;
    
};

#endif

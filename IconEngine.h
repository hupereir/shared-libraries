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
  static QIcon get( const std::string& file )
  { return get()._get( file ); }

  //! create icon
  static QIcon get( const QPixmap& pixmap )
  { return get()._get( pixmap ); }
  
  //! create icon
  static QIcon get( const QIcon& icon )
  { return get()._get( icon ); }
  
  //! map files and QIcon
  typedef std::map< std::string, QIcon > Cache;

  //! map files and QIcon
  class Pair: public std::pair<std::string, QIcon >, public Counter
  {
    
    public:
    
    //! constructor
    Pair( void ):
      Counter( "IconEngine::Pair" )
    {}
    
    //! constructor
    Pair( const std::pair<std::string, QIcon >& pair ):
      std::pair<std::string, QIcon >( pair ),
      Counter( "IconEngine::Pair" )
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
  
  private:
    
  //! constructor is private to force use of singleton
  IconEngine( void ):
    Counter( "IconEngine" )
  { Debug::Throw( "IconEngine::IconEngine.\n" ); } 
  
  //! singleton
  static IconEngine singleton_;
    
  //!@name non static methods are hidden
  //@{
 
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  QIcon _get( const std::string& file );

  //! create icon
  QIcon _get( const QPixmap& pixmap );
  
  //! create icon
  QIcon _get( const QIcon& icon );

  //@}
    
  //! map files and QIcon
  Cache cache_;
    
};

#endif

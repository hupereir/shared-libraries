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

#include "Counter.h"
#include "Debug.h"

//! customized Icon factory to provide better looking disabled icons
class IconEngine: public Counter
{
  
  public:
   
  //! create icon
  /*! the file is stored into a cache to avoid all pixmaps manipulations */
  static QIcon get( const std::string& file, const std::list<std::string> path_list );

  //! create icon
  static QIcon get( const QPixmap& pixmap );
  
  //! create icon
  static QIcon get( const QIcon& icon );
  
  //! clear cache
  static void clear( void )
  { cache_.clear(); }
  
  //! set the use_cache flag
  void setUseCache( const bool& value )
  { use_cache_ = value; }
  
  private:
    
  //! use cache to store icons generated from filenames
  static bool use_cache_;
  
  //! map files and QIcon
  static std::map< std::string, QIcon > cache_;
  
};

#endif

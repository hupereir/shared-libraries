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
   \file    Exception.h
   \brief   utility to tag exception description with the function, file and line
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#ifndef _exception_h_
#define _exception_h_
#include <string>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Debug.h"

//! Embedding macro
#define DESCRIPTION(message) std::string("Exception: ") + \
std::string(message) + ExceptionSuffix::get(__func__,__FILE__,__LINE__)

//! create exception suffix
class ExceptionSuffix {
 public:
 
  //! formats exception suffix using function, file and line number information
  static std::string get(std::string function, std::string file, int i){
    std::ostringstream o;
    o <<"  function:" << function << " file:" << file << " " << "line:" << i;
    return o.str();
  }
};

//! pure static exception class
class Exception {
  public:
  
  //! set debug level
  static void setDebugLevel( int debug_level )
  { debug_level_ = debug_level; }
      
  //! check pointer, throw exception if null
  static void checkPointer( const void* pointer, const std::string& message )
  { 
    if( !pointer ) 
    {
      Debug::Throw( debug_level_ ) << message << std::endl;
      throw std::logic_error( message );
    } 
  }
  
  //! check pointer, throw exception if null
  static void assert( const bool& value, const std::string& message )
  { 
    if( !value ) 
    {
      Debug::Throw( debug_level_ ) << message << std::endl;
      throw std::logic_error( message ); 
    }
  }

  private:
      
  //! default debug level
  static int debug_level_;
  
};

#endif

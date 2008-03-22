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

//! Embedding macro
#define DESCRIPTION(message) std::string("Exception: ") + \
std::string(message) + ExceptionSuffix::get(__func__,__FILE__,__LINE__)

/* Suffix class for appending __FILE__ and __LINE__ macros 
   to description string */
class ExceptionSuffix {
 public:
  
  //! create string from function suffix (retrieved via preprocessing information)
  static std::string get(std::string func, std::string file, int i){
    std::ostringstream tmp;
    tmp << " function: " << func << " file:" << file << " line:" << i;
    return tmp.str();
  }
  
};

#endif

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

#ifndef Debug_h
#define Debug_h

/*!
   \file    Debug.h
   \brief   option driven debugging facility 
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <iostream>
#include <fstream>
#include <string>

/*!
   \class   Debug
   \brief   option driven debugging class 
*/

class Debug{
  public:
  
  //! writes string to cout if level is lower than level_
  static void Throw( int level, const std::string& str )
  {
  
    if( level_ < level ) return;  
    std::cout << str;  
    return;
  
  }
  
  //! writes string to cout if level_ is bigger than 0
  static void Throw( const std::string& str ) 
  { Throw( 1, str ); }

  //! returns either cout or dummy stream depending of the level
  static std::ostream& Throw( int level = 1 )
  { return ( level_ < level ) ? nullStream_ : std::cout; }
  
  //! sets the debug level. Everything thrown of bigger level is not discarded
  static void setLevel( int level ) 
  { level_ = level; }
  
  //! retrieves the debug level
  static int level( void ) 
  { return level_; }
  
  private:
  
  //! debug level
  static int level_;  
  
  //! dummy stream to discard the text of two high debug level
  static std::ofstream nullStream_;
};

#endif

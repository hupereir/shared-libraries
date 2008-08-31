#ifndef Counter_h
#define Counter_h

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
   \file    Counter.h
   \brief   Object counter
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <map>
#include <string>
#include <stdexcept>
#include <iostream>

#include "Exception.h"

/*!
   \class   Counter
   \brief   Object counter
*/
class Counter {

  public:
  
  //! counter name and counts pair
  typedef std::pair<std::string, int> Pair;
      
  //! constructor
  Counter( const std::string& name );
  
  //! constructor
  Counter( const Counter& counter );

  //! destructor
  virtual ~Counter( void )
  { if( count_ ) (*count_)--; }
  
  //! retrieves counter count
  int count( void ) const
  { return (count_) ? *count_:0; }
  
  //! prints all counters to stream
  static void print( std::ostream& o = std::cout );
    
  private:
  
  //! pointer to integer counter
  int* count_;
    
};

#endif

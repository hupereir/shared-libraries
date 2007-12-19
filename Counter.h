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

#include <assert.h>
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
  
  //! counter map between name and existing objects
  class CounterMap: public std::map< std::string, int >        
  {
    public:
                      
    //! get counter for a given name
    /*! 
      if the name is found, returns adress of the existing counter
      creates new counter otherwise and returns adress
    */
    int* counter( const std::string& name )
    {
      iterator iter = find( name );
      if( iter == end() )
      return &(insert( std::pair<std::string, int>( name, 0 ) ).first->second);
      else return &(iter->second);
    }    
    
  };
  
  //! constructor
  Counter( void )
  { throw std::runtime_error( DESCRIPTION( "invalid constructor" ) ); }
    
  //! constructor
  Counter( const std::string& name ):
    name_( name ),
    count_( 0 )
  {
    count_ = counts_.counter( name );
    (*count_) ++;
    return;
  }

  
  //! constructor
  Counter( const Counter& counter ):
    name_( counter.name_ ),
    count_( 0 )
  {
    count_ = counts_.counter( name_ );
    (*count_) ++;
    return;
  }

  //! destructor
  virtual ~Counter( void )
  { if( count_ ) (*count_)--; }
  
  //! retrieves counter name
  const std::string& name( void ) const 
  { return name_; }
  
  //! retrieves counter count
  int count( void ) const
  { return (count_) ? *count_:0; }
  
  //! prints all counters to stream
  static void print( std::ostream& o = std::cout );
  
  //! returns counter map
  static CounterMap& map( void )
  { return counts_; }
  
  private:
  
  //! the object name associated to the counter
  std::string name_; 
  
  //! pointer to integer counter
  int* count_;
    
  //! the number of counts in this counter
  static CounterMap counts_;

};

#endif

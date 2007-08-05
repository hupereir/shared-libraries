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

#ifndef Str_h
#define Str_h

/*!
   \file Str.h
   \brief Enhanced stl string
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <cstdarg>
#include <sstream>
#include <string>
#include <list>
#include "Debug.h"
#include "Counter.h"

//! Enhanced stl string
class Str: public std::string, public Counter 
{
  public:

  //! empty string.
  static const std::string EMPTY_STRING;
    
  //! alignment enumeration for Str::Fill
  enum Alignment { LEFT = 0, RIGHT = 1, CENTER = 2 };
  
  //! constructor
  Str( void ):
    std::string( EMPTY_STRING ),
    Counter( "Str" )
  {}
  
  //! constructor
  Str( const std::string& value ):
    std::string( value ),
    Counter( "Str" )
  {}
  
  //! returns cat of str + spaces until it matches size.
  virtual Str fill( size_t size, Alignment alignment=LEFT ) const;
            
  //! replace all occurences of c1 with c2
  virtual Str replace( const std::string& c1, const std::string& c2 ) const;
    
  //! returns a copy of a string, with all characters converted to UpperCase
  virtual Str upperCase( void ) const;
    
  //! returns a copy of a string, with all characters converted to LowerCase
  virtual Str lowerCase( void ) const;
    
  //! returns first instance of a key located immediatly after given position in input string
  /*! 
    returns first instance of a key located immediatly after given position in input string
  */
  virtual size_t findAfter( const char key, size_t pos ) const;
    
  //! returns last instance of a key located immediatly before given position in input string
  /*! 
     returns last instance of a key located immediatly before given position in input string
    returns string::npos if not found or failure
  */
  virtual size_t findBefore( const char key, size_t pos ) const;
    
  //! returns first instance of any char in key located immediatly after given position in input string
  /*! 
    returns first instance of any char in key located immediatly after given position in input string
    returns string::npos for any failure.
  */
  virtual size_t findAnyAfter( const std::string& key, size_t pos ) const;
    
  //! returns last instance of any char in key located right before given position in input string
  /*! 
    returns last instance of any char in key located right before given position in input string
    returns string::npos for any failure.
  */
  virtual size_t findAnyBefore( const std::string& key, size_t pos ) const;
  
  //! returns position of last character on the right matching current one in the string
  virtual size_t findSameAfter( size_t ) const;
  
  //! returns position of last character on the left matching current one in the string
  virtual size_t findSameBefore( size_t ) const;
                       
  //! returns true if string is in argument
  virtual bool isIn( const Str& c2, bool caseSensitive = true ) const;
                       
  //! returns true if c1 == c2 
  virtual bool isEqual( const Str& c2, bool caseSensitive = true ) const;

  //! returns true if current string is lexicographicaly lower than the second
  virtual bool isLower( const Str& c2, bool caseSensitive = true ) const;
           
  //! returns the number of time argument string is current
  virtual unsigned int contains( const Str& c2, bool caseSensitive = true ) const;
                       
  //! returns true if string is in current
  virtual bool isIn( const char* c2, bool caseSensitive = true ) const
  { return (c2)? isIn( Str(c2), caseSensitive ):true; }
                       
  //! returns true if c1 == c2 
  virtual bool isEqual( const char* c2, bool caseSensitive = true ) const
  { return (c2)? isEqual( Str(c2), caseSensitive ):false; }

  //! returns true if current string is lexicographicaly lower than the second
  virtual bool isLower( const char* c2, bool caseSensitive = true ) const
  { return (c2)? isLower( Str(c2), caseSensitive ):true; }
           
  //! returns the number of time argument string is current
  virtual unsigned int contains( const char* c2, bool caseSensitive = true ) const
  { return (c2)? contains( Str(c2), caseSensitive ):0; }
  
  //! append a string to another
  Str& append( const Str& str )
  { 
    *this += str;
    return *this;
  }
  
  //! convert anything to string and fill
  template <typename T> 
  Str& assign( T value )
  {
    std::ostringstream o;
    o << value;
    if( o.rdstate() & std::ios::failbit ) {
      *this = EMPTY_STRING; 
      return *this;
    }
    
    *this = o.str();
    return *this;
  }
  
  //! convert anything to string and fill
  template <typename T> 
  Str& append( T value )
  { return append( Str().assign<T>( value ) ); }
      
  //! convert string to any type using string streamer
  template <typename T>
  T get( bool* error = 0 ) const
  {
    if( error ) *error = false;
    T out; 
    std::istringstream i( *this );
    i >> out;
  
    if( i.rdstate() & std::ios::failbit ) {
      if( error ) *error = true;
      return T();
    }
    
    return out;
  }
   
};

#endif

#ifndef Option_h
#define Option_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
   \file Option.h
   \brief Option objects for string, int, bool and double options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Counter.h"
#include "Debug.h"

/*!
   \class   Option
   \brief   Option objects for string, int, bool and double options
*/
class Option:public Counter {
  public:

  //! default creator
  Option(): 
    Counter( "Option" ),
    value_( "" ),
    comments_( "" ),
    flags_( RECORDABLE ),
    set_( false )
  {}
    
  //! default creator
  Option( const char* value, const unsigned int& flags = RECORDABLE ):
    Counter( "Option" ),
    comments_( "" ),
    flags_( flags ),
    set_( false )
  {
    assert( value );
    value_ = value;
    set_ = !value_.empty();
  }
  
  //! filled creator
  Option( const std::string& value, const unsigned int& flags = RECORDABLE ): 
    Counter( "Option" ),
    value_( value ),
    comments_( "" ),
    flags_( flags ),
    set_( !value.empty() )
  {}
 
  //! filled creator
  Option( const std::string& value, const std::string& comments, const unsigned int& flags = RECORDABLE ): 
    Counter( "Option" ),
    value_( value ),
    comments_( comments ),
    flags_( flags ),
    set_( !value.empty() )
  {}
  
  //! less than operator
  bool operator < (const Option& option ) const
  { return value_ < option.value_; }

  //! equal operator
  bool operator == (const Option& option ) const
  { return value_ == option.value_; }

  //! option comments
  const std::string& comments( void ) const
  { return comments_; }
  
  //! option comments
  Option& setComments( const std::string& comments )
  { 
    comments_ = comments; 
    return *this;
  }
  
  //!@name flags
  //@{
  
  //! flags
  enum Flag
  {
    NONE = 0,
    DEFAULT = 1<<0,
    RECORDABLE = 1<<1
  };
  
  //! flags
  Option& setFlags( unsigned int value )
  { 
    flags_ = value; 
    return *this;
  }
  
  //! flags
  Option& setFlag( Flag flag, const bool& value = true )
  {
    if( value ) { flags_ |= flag; }
    else { flags_ &= (~flag); } 
    return *this;
  }
  
  //! flags
  const unsigned int& flags( void ) const
  { return flags_; }

  //! flags
  bool hasFlag( const Flag& flag ) const
  { return flags_ & flag; }

  //@}

  //! raw accessor
  const std::string& raw( void ) const
  { return value_; }

  //! raw modifier
  Option& setRaw( const std::string& value )
  { 
    value_ = value; 
    return *this;
  }

  //! accessor
  template < typename T >
  T get( void ) const
  {
    
    // check if option is set
    if( !set_ ) 
    {
      std::ostringstream o; 
      o << "option not set";
      throw std::logic_error( DESCRIPTION(o.str()) );
    }
    
    // cast value
    std::istringstream s( value_ );
    T out; s >> out;
    if( s.rdstate() & std::ios::failbit ) 
    {
      std::ostringstream o; 
      o << "Wrong type for registered value=" << value_;
      throw std::logic_error( DESCRIPTION(o.str()) );
    }
    return out;
  }
  
  //! modifier
  template < typename T >
  Option& set( const T& value )
  {
    std::ostringstream s; s << value;
    value_ = s.str();
    set_ = true;
    return *this;
  }
  
  //! check status
  const bool& set( void ) const {return set_;}

  //! method used to dump the option to stream
  friend std::ostream &operator << (std::ostream &o,const Option &opt)
  {
    if( !opt.set() ) o << "not set";
    else o << opt.raw();
    return o;
  }   
  
  protected:
  
  //! validity
  void _setValid( const bool& value )
  { set_ = value; }
  
  private:
  
  //! option value
  std::string value_;
  
  //! option comments
  std::string comments_; 
  
  //! flags
  unsigned int flags_;
  
  //! true if option was initialized with value  
  bool set_;         
  
};

#endif

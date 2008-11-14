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
    flags_( RECORDABLE )
  {}
    
  //! default creator
  Option( const char* value, const unsigned int& flags = RECORDABLE ):
    Counter( "Option" ),
    flags_( flags )
  {
    assert( value );
    value_ = value;
  }
  
  //! filled creator
  Option( const std::string& value, const unsigned int& flags = RECORDABLE ): 
    Counter( "Option" ),
    value_( value ),
    flags_( flags )
  {}
 
  //! filled creator
  Option( const std::string& value, const std::string& comments, const unsigned int& flags = RECORDABLE ): 
    Counter( "Option" ),
    value_( value ),
    comments_( comments ),
    flags_( flags )
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
    CURRENT = 1 << 0,
    RECORDABLE = 1<<1
    //DEFAULT = 1<<2,
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
  
  //! used to retrieve file records that match a given flag
  class HasFlagFTor
  {
    
    public:
    
    //! constructor
    HasFlagFTor( const unsigned int& flag ):
      flag_( flag )
      {}
      
    //! predicate
    bool operator() ( const Option& option ) const
    { return option.flags() & flag_; }
      
    private:
    
    // predicted flag
    unsigned int flag_;
    
  };

  //! current
  bool isCurrent( void ) const
  { return flags_ & CURRENT; }
  
  //! current
  void setCurrent( const bool& value )
  { setFlag( CURRENT, value ); }
  
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
    assert( !value_.empty() ); 
    
    // cast value
    std::istringstream s( value_ );
    T out; s >> out;
    assert( !(s.rdstate() & std::ios::failbit ) ); 
    return out;
  }
  
  //! modifier
  template < typename T >
  Option& set( const T& value )
  {
    std::ostringstream s; s << value;
    value_ = s.str();
    return *this;
  }
  
  //! check status
  const bool& set( void ) const 
  {return !value_.empty();}

  //! method used to dump the option to stream
  friend std::ostream &operator << (std::ostream &o,const Option &opt)
  {
    if( !opt.set() ) o << "not set";
    else o << opt.raw();
    return o;
  }   
  
  //! restore default value
  void restoreDefault()
  {}
    
  private:
  
  //! option default value
  std::string default_value_;
  
  //! option value
  std::string value_;
  
  //! option comments
  std::string comments_; 
  
  //! flags
  unsigned int flags_;
  
};

#endif

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
  Option();
    
  //! default creator
  Option( const char* value, const unsigned int& flags = RECORDABLE );
  
  //! filled creator
  Option( const std::string& value, const unsigned int& flags = RECORDABLE );
 
  //! filled creator
  Option( const std::string& value, const std::string& comments, const unsigned int& flags = RECORDABLE );
  
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
      
    //! sorting predicate
    /*! it is used to ensure that options that have a given flag appear first in a list */
    bool operator() (const Option& first, const Option& second ) const
    { return ( (first.flags() & flag_) && !(second.flags()&flag_) ); }
    
    private:
    
    // predicted flag
    unsigned int flag_;
    
  };

  //! current
  bool isCurrent( void ) const
  { return hasFlag( CURRENT ); }
  
  //! current
  Option& setCurrent( const bool& value )
  { return setFlag( CURRENT, value ); }
    
  //@}

  //! default
  Option& setDefault( void )
  { 
    default_value_ = value_;
    default_flags_ = flags_;
    return *this;
  }
  

  //! raw accessor
  const std::string& raw( void ) const
  { return value_; }

  //! raw modifier
  Option& setRaw( const std::string& value )
  { 
    value_ = value; 
    return *this;
  }
  
  //! default value
  const std::string& defaultValue( void ) const
  { return default_value_; }

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
  bool set( void ) const 
  {return !value_.empty();}

  //! method used to dump the option to stream
  friend std::ostream &operator << (std::ostream &o,const Option &opt)
  {
    if( !opt.set() ) o << "not set";
    else o << opt.raw();
    return o;
  }
  
  //! restore default value
  Option& restoreDefault()
  { 
    value_ = default_value_;
    flags_ = default_flags_;
    return *this;
  }
    
  private:
  
  //! option value
  std::string value_;
  
  //! option comments
  std::string comments_; 
  
  //! flags
  unsigned int flags_;
  
  //! option default value
  std::string default_value_;
 
  //! default flags
  unsigned int default_flags_;
  
};

#endif

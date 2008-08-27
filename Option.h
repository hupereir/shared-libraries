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

#ifndef Option_h
#define Option_h

/*!
   \file Option.h
   \brief Option objects for string, int, bool and double options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

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
    name_( "" ),
    value_( "" ),
    comments_( "" ),
    set_( false ),
    recordable_( true ),
    front_( false )
  {}
    
  //! filled creator
  Option( 
    const std::string& name, 
    const std::string& value, 
    const std::string& comments = "",
    bool front = false ): 
    Counter( "Option" ),
    name_( name ),
    value_( value ),
    comments_( comments ),
    set_( value.size() ),
    recordable_( true ),
    front_( front )
  {}

  //! less than operator
  bool operator < (const Option& option ) const
  { 
    if( name_ < option.name_ ) return true; 
    else if( name_ == option.name_ ) return value_ < option.value_;
    else return false;
  }

  //! equal operator
  bool operator == (const Option& option ) const
  { 
    bool out( 
      name_ == option.name_ && 
      value_ == option.value_ 
      ); 
    return out;
  }

  //! option name
  const std::string& name( void ) const
  { return name_; }

  //! option name
  Option& setName( const std::string& name )
  { 
    name_ = name; 
    return *this;
  }

  //! option comments
  const std::string& comments( void ) const
  { return comments_; }
  
  //! option comments
  Option& setComments( const std::string& comments )
  { 
    comments_ = comments; 
    return *this;
  }

  //! returns true if option needs to be stored front in option list
  const bool& front( void ) const
  { return front_; }

  //! returns true if option needs to be stored front in option list
  void setFront( const bool& value )
  { front_ = value; }
  
  //! check if option is to be saved in resource file
  const bool& isRecordable( void ) const
  { return recordable_; }

  //! set option recordability
  Option& setRecordable( bool value ) 
  { 
    recordable_ = value;
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

  //! accessor
  template < typename T >
  T get( void ) const
  {
    std::ostringstream o; 
    o << "option " << name_ << " not set";
    if( !set_ ) throw std::logic_error( DESCRIPTION(o.str()) );
  
    std::istringstream s( value_ );
    T out; s >> out;
    if( s.rdstate() & std::ios::failbit ) {
      std::ostringstream o; 
      o << "Wrong type for registered option " << name_ << ", value=" << value_;
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
    o << opt.name() << " : ";
    if( !opt.set() ) o << "not set";
    else o << opt.raw();
    return o;
  }   
  
  protected:
  
  //! validity
  void _setValid( const bool& value )
  { set_ = value; }
  
  private:
  
  //! option name
  std::string name_;
  
  //! option value
  std::string value_;
  
  //! option comments
  std::string comments_; 
  
  //! true if option was initialized with value  
  bool set_;         
  
  //! true if option is to be saved into resource file
  bool recordable_;
  
  //! if true, must be added front in special option list
  bool front_;
  
};

#endif

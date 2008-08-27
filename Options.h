#ifndef Options_h
#define Options_h

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
  \file Options.h
  \brief Option file parser based on xml
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

#include "Counter.h"
#include "Debug.h"
#include "Option.h"

//! Option file parser based on Xml
class Options: public Counter 
{

  public: 
   
  //! shortCut for option map
  typedef std::map< std::string, Option > Map;

  //! shortCut for option list
  typedef std::list< Option > List;
        
  //! shortCut for option map
  typedef std::map< std::string, List > SpecialMap;
    
  //! constructor
  Options( void );
  
  //! destructor
  virtual ~Options( void ) 
  {}
  
  //! equality operator
  bool operator == (const Options& options ) const
  {
    Debug::Throw( "Options::operator ==.\n" );
    bool out(
        ( options_ == options.options_ ) &&
        ( special_options_ == options.special_options_ ) );
    return out;
  }

  //! adds a new option. Return true if option is added
  virtual bool add( const Option& option, bool overwrite = true );
  
  //! retrieve list of special (i.e. kept) options
  virtual const SpecialMap& specialOptions() const
  { return special_options_; }
  
  //! retrieve list of special (i.e. kept) options matching a given name
  virtual List& specialOptions( const std::string& name )
  { return special_options_[name]; }
  
  //! returns true if option name is special
  virtual bool isSpecialOption( const std::string& name ) const
  { return special_options_.find( name ) != special_options_.end(); }
  
  //! retrieve list of special (i.e. kept) options matching a given name
  template < typename T > 
  std::list<T> specialOptions( const std::string& name )
  {
    
    List option_list( specialOptions( name ) );
    std::list<T> out;
    for( List::iterator iter = option_list.begin(); iter != option_list.end(); iter++ )
    out.push_back( iter->get<T>() );
    return out; 
    
  }
  
  //! clear list of special (i.e. kept) options matching a given name
  virtual void clearSpecialOptions( const std::string& name );
  
  //! returns all options
  const Map& options( void ) const
  { return options_; }
  
  //! returns true if option with matching name is found
  virtual bool find( const std::string& name )
  { return options_.find( name ) != options_.end(); }
 
  //! option accessor
  virtual Option& option( const std::string& name );
 
  //! option value accessor
  template < typename T >
  T get( const std::string& name )
  { return option( name ).get<T>(); }

  //! option raw value accessor
  virtual std::string raw( const std::string& name )
  { return option( name ).raw(); }

  //! option value modifier
  template < typename T >
  void set( const std::string& name, const T& value )
  { 
    if( !find( name ) ) add( Option( name, "" ) );
    option( name ).set<T>( value ); 
  }

  //! option raw value modifier
  virtual void setRaw( const std::string& name, const std::string& value )
  { 
    if( find( name ) ) option( name ).setRaw( value ); 
    else add( Option( name, value ) );
  }
  
  /*! \brief
    tags a given option to be kept in separate list as a "special" option. 
    These options are not stored unique. 
    They are stored in a list, with no unicity check. Accessing these therefore
    takes more time with respect to the "standard" options, which are stored
    in a map.
  */
  virtual void keep( const std::string& name )
  { 
    if( special_options_.find( name ) == special_options_.end() )
    special_options_.insert( make_pair( name, List() ) );
  }
    
  //! dump options to stream
  virtual void dump( std::ostream& out = std::cout ) const;
  
  //! retrieve Option map
  virtual Map& map( void )
  { return options_; }
    
  private:
   
  //! option map
  Map options_;
  
  //! set of option names to be kept separately
  SpecialMap special_options_;
        
};

#endif

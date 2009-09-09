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

#include <cassert>

#include <vector>
#include <map>
#include <list>

#include <QString>
#include <QTextStream>

#include "Counter.h"
#include "Debug.h"
#include "Option.h"

//! Option file parser based on Xml
class Options: public Counter
{

  public:

  //! pair
  typedef std::pair< QString, Option > Pair;

  //! shortCut for option map
  typedef std::map< QString, Option > Map;

  //! shortCut for option list
  typedef std::vector< Option > List;

  //! shortCut for option map
  typedef std::map< QString, List > SpecialMap;

  //! constructor
  Options( bool install_default_options = false );

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

  //! retrieve Option map
  const virtual Map& options( void ) const
  { return options_; }

  //! retrieve special Option map
  const virtual SpecialMap& specialOptions( void ) const
  { return special_options_; }

  //! adds a new option. Return true if option is added
  virtual bool add( const QString&, Option, const bool& is_default = false );

  //! retrieve list of special (i.e. kept) options matching a given name
  virtual List& specialOptions( const QString& name )
  {
    SpecialMap::iterator iter( special_options_.find( name ) );
    assert( iter != special_options_.end() );
    return iter->second;
  }

  //! returns true if option name is special
  virtual bool isSpecialOption( const QString& name ) const;

  //! retrieve list of special (i.e. kept) options matching a given name
  template < typename T >
  std::list<T> specialOptions( const QString& name )
  {

    List option_list( specialOptions( name ) );
    std::list<T> out;
    for( List::iterator iter = option_list.begin(); iter != option_list.end(); iter++ )
    { out.push_back( iter->get<T>() ); }
    return out;

  }

  //! clear list of special (i.e. kept) options matching a given name
  virtual void clearSpecialOptions( const QString& name );

  //! returns true if option with matching name is found
  virtual bool find( const QString& name )
  { return options_.find( name ) != options_.end(); }

  //! option matching given name
  virtual const Option& option( const QString& name ) const
  { return options_.find( name )->second; }

  //! option value accessor
  template < typename T >
  T get( const QString& name ) const
  { return _find( name )->second.get<T>(); }

  //! option raw value accessor
  virtual QString raw( const QString& name ) const
  { return _find( name )->second.raw(); }

  //! option value modifier
  void set( const QString& name, Option option, const bool& is_default = false );

  //! option value modifier
  template < typename T >
  void set( const QString& name, const T& value, const bool& is_default = false )
  {
    assert( !isSpecialOption( name ) );
    Option &option( options_[name] );
    option.set<T>( value );
    if( is_default || _autoDefault() ) option.setDefault();

  }

  //! option raw value modifier
  virtual void setRaw( const QString& name, const QString& value, const bool& is_default = false )
  {
    assert( !isSpecialOption( name ) );
    Option &option( options_[name] );
    option.setRaw( value );
    if( is_default || _autoDefault() ) option.setDefault();
  }

  /*! \brief
    tags a given option to be kept in separate list as a "special" option.
    These options are not stored unique.
    They are stored in a list, with no unicity check. Accessing these therefore
    takes more time with respect to the "standard" options, which are stored
    in a map.
  */
  virtual void keep( const QString& name )
  {
    if( special_options_.find( name ) == special_options_.end() )
    special_options_.insert( make_pair( name, List() ) );
  }

  //! auto-default
  void setAutoDefault( const bool& value )
  { auto_default_ = value; }

  //! restore defaults
  void restoreDefaults( void );

  //! print
  void print( void ) const
  {
    QTextStream what( stdout );
    what << *this;
  }

  protected:

  //! find name
  Map::const_iterator _find( const QString& name ) const;

  //! auto-default
  const bool& _autoDefault( void ) const
  { return auto_default_; }

  private:

  //! option map
  Map options_;

  //! set of option names to be kept separately
  SpecialMap special_options_;

  //! if true all options inserted are also set as default
  bool auto_default_;

  //! streamer
  friend QTextStream &operator << ( QTextStream &,const Options &);

};

#endif

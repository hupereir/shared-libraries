#ifndef CommandLineParser_h
#define CommandLineParser_h

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
  \file CommandLineParser.h
  \brief stores command line arguments
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QStringList>
#include <map>

#include "CommandLineArguments.h"
#include "Counter.h"
#include "Debug.h"

/*! \brief parse command line argument. */
class CommandLineParser: public Counter
{

  public:
  
  //! constructor
  CommandLineParser();
  
  //! register option
  void registerOption( QString tag, QString type, QString help_text );

  //! register flag
  void registerFlag( QString tag, QString help_text );
  
  //! print help
  void usage( void ) const;
  
  //! parse
  CommandLineParser& parse( const CommandLineArguments&, bool ignore_warnings = true );
  
  //! clear
  void clear( void );
  
  //! return 'rectified' arguments
  CommandLineArguments arguments( void ) const;
  
  //! application name
  QString applicationName( void ) const
  { return application_name_; }
  
  //! return true if flag is on
  bool hasFlag( QString ) const;
  
  //! return true if option is set
  bool hasOption( QString ) const;
  
  //! return option associated to tag
  QString option( QString ) const;
  
  //! orphans
  /*! list of command line arguments located at the end of the list and that do not match any option */
  QStringList& orphans( void ) 
  { return orphans_; }
  
  private:
  
  //! discard orphans
  void _discardOrphans( bool ignore_warnings );
  
  //! returns true if string is a tag
  bool _isTag( QString ) const;
  
  //! flag class
  class Flag: public Counter
  {
    
    public: 
    
    //! constructor
    Flag( QString help_text = QString() ):
      Counter( "CommandLineParser::Flag" ),
      help_text_( help_text ),
      set_( false )
    {}
    
    //! destructor
    virtual ~Flag( void )
    {}
        
    //! help text
    QString help_text_;
    
    //! true if set
    bool set_;
    
  };
  
  //! option class
  class Option: public Flag
  {
    
    public:
    
    //! constructor
    Option( QString type = QString(), QString help_text = QString() ):
      Flag( help_text ),
      type_( type )
    {}
    
    //! type
    QString type_;
    
    //! value
    QString value_;
    
  };
  
  //! flags
  typedef std::map<QString, Flag> FlagMap;
  
  //! flags
  FlagMap flags_;
  
  //! options
  typedef std::map<QString, Option> OptionMap;
  
  //! used to select tag of maximum length
  class MinLengthFTor
  {
    
    public:
    
    //! predicate
    bool operator () ( const FlagMap::value_type& first, const FlagMap::value_type& second )
    { return first.first.size() < second.first.size(); }
  
    //! predicate
    bool operator () ( const OptionMap::value_type& first, const OptionMap::value_type& second )
    { return first.first.size() < second.first.size(); }
    
  };
  
  //! used to select tag of maximum length
  class MinTypeLengthFTor
  {
    
    public:
      
    //! predicate
    bool operator () ( const OptionMap::value_type& first, const OptionMap::value_type& second )
    { return first.second.type_.size() < second.second.type_.size(); }
    
  };

  //! options
  OptionMap options_;
  
  //! application name
  QString application_name_;
  
  //! orphan arguments 
  QStringList orphans_;
  
};

#endif

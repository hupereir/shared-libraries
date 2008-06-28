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

#ifndef Util_h
#define Util_h

/*!
   \file    Util.h
   \brief   some utilities used everywhere
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/
#include <QStringList>
#include <string>
#include <list>

/*!
   \class   Util
   \brief   some utilities used everywhere
*/
class Util
{
  public:

  //! square a number
  template< typename T > 
  static T sqr( const T& t)
  { return t*t; }
        
  /*! 
    returns time stamp associated to current date
    format is dd-mm-yy_hh-mm-ss
  */
  static std::string timeStamp( void );
    
  /*! 
    returns time string associated to current date
    format is dd/mm/yy hh:mm:ss
  */
  static std::string timeString( void );
   
  //! runs a Unix command 
  static bool run( const std::string& val )
  { return run( QString( val.c_str() ) ); }

  //! runs a Unix command 
  static bool run( const QString& val );
  
  //! runs a Unix command 
  static bool run( QStringList arguments );
   
  //! runs a Unix command in specified directory
  static void runAt( const std::string& path, const std::string& val );
   
  //! runs a Unix command in specified directory
  static void runAt( const std::string& path, QStringList arguments );
   
  //! returns all children processes starting from pid. [recursive]
  static std::list< unsigned int > allChildren( const unsigned int pid_ );
  
  //! returns environment variable or empty string
  static std::string env( const std::string&, const std::string& default_value = "" );
  
  //!  user name
  static std::string user( void );
  
  //! returns the domain name
  static std::string domain( void );
  
  //! user home directory
  static std::string home( void );
    
  //! temporary directory ("/tmp")
  static std::string tmp( void );
  
  /*! returns the host name. 
    \par short_name if true returns only the string located before the first '.'
  */
  static std::string host( bool short_name = false );  
  
  //! application pid
  static int pid( void );  
  
  //! returns the work directory
  static std::string workingDirectory( void );  

  //! converts a time in second into an hour/min/sec character string
  static std::string convertTime( float value );
  
};

#endif

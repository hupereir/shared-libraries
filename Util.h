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
       
  //! returns environment variable or empty string
  static QString env( const QString&, const QString& default_value = "" );
  
  //!  user name
  static QString user( void );
  
  //! returns the domain name
  static QString domain( void );
  
  //! user home directory
  static QString home( void );
    
  //! temporary directory ("/tmp")
  static QString tmp( void );
  
  /*! returns the host name. 
    \par short_name if true returns only the string located before the first '.'
  */
  static QString host( bool short_name = false );  
  
  //! application pid
  static int pid( void );  
  
  //! returns the work directory
  static QString workingDirectory( void );  

};

#endif

#ifndef Singleton_h
#define Singleton_h

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
   \file    Singleton.h
   \brief   a class singleton used to centralize all objects that need static creation
   this allows that these objects get deleted in the proper order when leaving the application
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <assert.h>
#include <QObject>

#include "CounterMap.h"

//! a class singleton used to centralize all objects that need static creation
class Singleton
{
  
  public:
  
  //! return singleton
  static Singleton& get( void );
  
  //! counter map
  CounterMap& counterMap( void ) 
  { return counter_map_; }
  
  //! set application
  void setApplication( QObject* application )
  { 
    assert( !application_ );
    application_ = application;
  }
  
  //! application
  QObject* application()
  { 
    assert( application_ );
    return application_; 
  }
  
  
  //! cast
  template< typename T >
  T* application( void )
  { 
    assert( application_ );
    return static_cast<T*>( application_ );  
  }
  
  //! destructor
  ~Singleton( void );
  
  private:
  
  //! constructor
  Singleton( void ):
    application_(0)
  {}
  
  //! counter map
  CounterMap counter_map_; 

  QObject* application_;
  
};

#endif

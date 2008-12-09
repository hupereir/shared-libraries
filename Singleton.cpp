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
   \file    Singleton.cpp
   \brief   a class singleton used to centralize all objects that need static creation
   this allows that these objects get deleted in the proper order when leaving the application
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Singleton.h"

// Singleton Singleton::singleton_;


//__________________________________________________
Singleton& Singleton::get( void )
{ 
  static Singleton singleton_;
  return singleton_; 
}


//__________________________________________________
Singleton::~Singleton( void )
{ counterMap().clear(); }

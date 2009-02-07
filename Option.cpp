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
   \file XmlOption.cpp
   \brief Xml implementation of the Option object
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Option.h"

//________________________________________________________
QTextStream& operator >> ( QTextStream& in, bool& value )
{
  short unsigned int local;
  in >> local;
  value = (bool)( local );
  return in;
}

//________________________________________________________
Option::Option(): 
  Counter( "Option" ),
  flags_( RECORDABLE ),
  default_flags_( RECORDABLE )
  {}
    
//________________________________________________________
Option::Option( const char* value, const unsigned int& flags ):
  Counter( "Option" ),
  flags_( flags ),
  default_flags_( RECORDABLE )
{
  assert( value );
  value_ = value;
}
  
//________________________________________________________
Option::Option( const QString& value, const unsigned int& flags ): 
  Counter( "Option" ),
  value_( value ),
  flags_( flags ),
  default_flags_( RECORDABLE )
{}

//________________________________________________________
Option::Option( const QString& value, const QString& comments, const unsigned int& flags ): 
  Counter( "Option" ),
  value_( value ),
  comments_( comments ),
  flags_( flags ),
  default_flags_( RECORDABLE )
{}

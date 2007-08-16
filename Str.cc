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
   \file    Str.cc
   \brief   some utilities to handle strings safely
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Str.h"
#include "Debug.h"
#include "Exception.h"

#include <iostream>
#include <sstream>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

using namespace std;

//______________________________________________________________________
Str Str::fill( size_t end_size, Str::Alignment alignment ) const
{ 
  if( size() >= end_size ) return *this;
  size_t diff = end_size - size();
  
  switch( alignment ) {
    case LEFT: return (*this)+string( diff, ' '); 
    case RIGHT: return string( diff, ' ')+(*this); 
    case CENTER: return string( (diff)/2, ' ')+(*this)+string(diff-size_t(diff/2), ' ');
    default: return *this;
  }   
}

//______________________________________________________________________
Str Str::replace( const string& c1, const string& c2 ) const 
{
  if( !c1.size() ) return Str();
  Str out("");
  size_t len = size();
  size_t current = 0;
  size_t found=0;
  while( current < len && ( found = find( c1, current ) ) != string::npos ) 
  {
    out += substr( current, found-current ) + c2;
    current=found+c1.size();
  }
  
  if( current < len )
  out += substr( current, len-current );
  
  return out;
}
     
//____________________________________________  
Str Str::upperCase( void ) const
{  
  unsigned int len = size();
  Str out( *this );
  for( unsigned int i=0; i<len; i++ )
  out[i] = toupper( (*this)[i] );
  return out;
}
     
//____________________________________________  
Str Str::lowerCase( void ) const
{  
  unsigned int len = size();
  string out( *this );
  for( unsigned int i=0; i<len; i++ )
  out[i] = tolower( (*this)[i] );
  return out;
}

//_______________________________________________
size_t Str::findAfter( char key, size_t pos ) const
{
  if( pos >= size() ) return string::npos;
  
  size_t out = find( key, pos );
  if( out == string::npos ) out = size();
  return out;
}  

//_______________________________________________
size_t Str::findBefore( char key, size_t pos ) const
{
  if( !pos ) return string::npos;
  
  size_t out = rfind( key, pos );
  if( out == string::npos ) out = 0; 
  else out++;
  return out;
}  

//_______________________________________________
size_t Str::findAnyAfter( const string& key, size_t pos ) const
{
  if( pos >= size() ) return string::npos;  // check position
  
  size_t min = string::npos;
  for( size_t i = 0; i < key.size(); i++ ) {
    size_t found = find( key[i], pos );
    if( found != string::npos && ( min == string::npos || found < min ) ) min = found;
  }
  
  if( min==string::npos ) min=size();
  return min;
}  

//_______________________________________________
size_t Str::findAnyBefore( const string& key, size_t pos ) const
{
  if( !pos ) return string::npos;
  
  size_t max = string::npos;
  for( size_t i = 0; i < key.size(); i++ ) {
    size_t found = rfind( key[i], pos );
    if( found != string::npos && ( max == string::npos || found > max ) ) max = found;
  }

  if( max == string::npos ) max = 0; 
  else max++;
  return max;
}  

//_______________________________________________
size_t Str::findSameAfter( size_t current ) const
{
  if( current >= size() ) return current;
  
  size_t i;
  for( i = current; i < size() && (*this)[i]==(*this)[current]; i++ );
  return ((*this)[i] == (*this)[current])? i+1:i;
}


//_______________________________________________
size_t Str::findSameBefore( size_t current ) const
{
  if( current >= size() ) return current;
  
  int i;
  for( i = current; i >= 0 && (*this)[i] == (*this)[current]; i-- );
  return ((*this)[i]== (*this)[current])? i:i+1;
}

//____________________________________________  
bool Str::isIn( const Str& c2, bool caseSensitive ) const
{  
  if( !size() ) return true;
  
  Str C1( ( caseSensitive ) ? *this : upperCase() );
  Str C2( ( caseSensitive ) ? c2 : c2.upperCase() );
  return ( C2.find( C1 ) != string::npos );
}

//____________________________________________  
unsigned int Str::contains( const Str& c2, bool caseSensitive ) const
{  
  if( !c2.size() ) return true;
  
  Str C1( ( caseSensitive ) ? *this : upperCase() );
  Str C2( ( caseSensitive ) ? c2 : c2.upperCase() );
  size_t begin = 0;
  size_t end = 0;
  unsigned int out = 0;
  while( ( end = C1.find( C2, begin ) ) != string::npos )
  {
    out++;
    begin = end + C2.size();
  }
  return out;
}

//____________________________________________  
bool Str::isEqual( const Str& c2, bool caseSensitive ) const
{  
  if( !size() ) return false;
  
  Str C1( ( caseSensitive ) ? *this : upperCase() );
  Str C2( ( caseSensitive ) ? c2 : c2.upperCase() );
  return ( C2 == C1 );
}

//____________________________________________  
bool Str::isLower( const Str& c2, bool caseSensitive ) const
{
  Str C1( ( caseSensitive ) ? *this : upperCase() );
  Str C2( ( caseSensitive ) ? c2 : c2.upperCase() );
  return C1<C2;
} 

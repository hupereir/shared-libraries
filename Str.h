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

#ifndef Str_h
#define Str_h

/*!
   \file Str.h
   \brief Enhanced stl string
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QString>
#include <QStringList>

#include "Debug.h"
#include "Counter.h"

//! Enhanced stl string
class Str: public QString, public Counter 
{
  public:
    
  //! constructor
  Str( void ):
    Counter( "Str" )
  {}
  
  //! constructor
  Str( const QString& value ):
    QString( value ),
    Counter( "Str" )
  {}
                                                  
  //! append a string to another
  Str& append( const QString& str )
  { 
    *this += str;
    return *this;
  }
  
  //! convert anything to string and fill
  template <typename T> 
  Str& assign( T value )
  {
    clear();
    QTextStream out( this, QIODevice::WriteOnly );
    out << value;
    
    if( out.status() != QTextStream::Ok ) 
    { clear(); }

    return *this;
  }
  
  //! convert anything to string and fill
  template <typename T> 
  Str& append( T value )
  { return append( Str().assign<T>( value ) ); }
      
  //! convert string to any type using string streamer
  template <typename T>
  T get( bool* error = 0 ) const
  {
    if( error ) *error = false;
    T out; 
    
    /* 
    the const cast here is justified by the ReadOnly flag
    while Qt does not allow to pass a const QString* in QTextStream constructor 
    */
    QTextStream in( const_cast<Str*>(this), QIODevice::ReadOnly );
    in >> out;
  
    if( in.status() != QTextStream::Ok ) {
      if( error ) *error = true;
      return T();
    } else return out;
    
  }
   
};

#endif

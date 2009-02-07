#ifndef XmlError_h
#define XmlError_h

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
* Place, Suite 330, Boston, MA 02111-1307 USA                           
*                         
*                         
*******************************************************************************/


/*!
   \file XmlError.h
   \brief Xml parsing error container
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <list>
#include <QString>
#include <QTextStream>

#include "Counter.h"

//! Xml parsing error container
class XmlError: public Counter
{
  
  public:
       
  //! constructor
  XmlError( const QString& file = QString() ):
    Counter( "XmlError" ),
    file_ ( file ),
    error_( "" ),
    line_(0),
    column_(0)
  {}
  
  //! shortcut to list of errors
  typedef std::list< XmlError > List;
    
  //! cast to boolean
  operator bool(void) const
  { return error_.length() != 0; }
  
  //! File
  const QString& file( void ) const
  { return file_; }
  
  //! error
  QString& error( void )
  { return error_; }
  
  //! error
  const QString& error( void ) const
  { return error_; }
  
  //! line
  int& line( void )
  { return line_; }
  
  //! line
  const int& line( void ) const
  { return line_; }
  
  //! column
  int& column( void )
  { return column_; }
  
  //! column
  const int& column( void ) const
  { return column_; }
  
  private:

  //! file
  QString file_;
            
  //! error
  QString error_;
  
  //! line
  int line_;
  
  //! column
  int column_;
  
  //! dumper
  friend QTextStream& operator << ( QTextStream &out, const XmlError& error )
  {
    if( !error.file().isEmpty() ) out << error.file() << ": ";
    else out << "message: ";
    out << error.error() << " at line " << error.line() << ", column " << error.column();
    return out;
  }
  
  //! dumper
  friend std::ostream& operator << ( std::ostream &out, const List& errors )
  {
    for( List::const_iterator iter = errors.begin(); iter != errors.end(); iter++ )
    out << *iter << std::endl;
    return out;
  }
    
};

#endif

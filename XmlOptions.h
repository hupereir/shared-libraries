#ifndef XmlOptions_h
#define XmlOptions_h

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
  \file XmlOptions.h
  \brief Option file parser based on xml
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "File.h"
#include "Options.h"
#include "XmlError.h"

//! Option file parser based on xml
class XmlOptions
{

  public:
  
  //! retrieve singleton
  static Options& get( void )
  { return singleton_; }
  
  //! read from file
  static bool read( File file = File( "" ) );

  //! write options to file
  static bool write( File file = File("") );
  
  //! file
  static const File& file()
  { return file_; }
  
  private:
  
  //! singleton
  static Options singleton_;
  
  //! file name
  static File file_;
  
  //! xml error
  static XmlError error_;
  
};

#endif

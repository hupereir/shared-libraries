
#ifndef OptionWidget_h
#define OptionWidget_h

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
   \file    OptionWidget.h
   \brief   base class for option manipulating widgets
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <string>

//! base class for option manipulating widgets
/*! 
  OptionWidgets are used as a base class to register widgets associated 
  to a given Option in a Configuration dialog. They have a string member, which
  is the option name, and 2 pure virtual functions for reading and writting the
  option. These must be reimplemented into specific widgets.
*/
class OptionWidget
{
  public:
      
  //! constructor
  OptionWidget( const std::string& name ):
    option_name_( name )
  {}
  
  //! destructor
  virtual ~OptionWidget()
  {}
  
  //! set widget value from option value
  virtual void read( void ) = 0;
  
  //! set option value from widget value
  virtual void write( void ) const = 0;
  
  protected:
  
  //! option name
  const std::string& optionName( void ) const
  { return option_name_; }
  
  private:
  
  //! name of the option linked to the widget    
  std::string option_name_;
  
};

#endif

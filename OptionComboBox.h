#ifndef OptionComboBox_h
#define OptionComboBox_h

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
   \file    OptionComboBox.h
   \brief   QComboBox associated to an option for configuration dialogs
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "CustomComboBox.h"
#include "OptionWidget.h"
#include "Options.h"

//! QComboBox associated to an option for configuration dialogs
class OptionComboBox: public CustomComboBox, public OptionWidget
{
  
  public:
  
  //! constructor
  OptionComboBox( QWidget* parent, const std::string& option_name ):
      CustomComboBox( parent ),
      OptionWidget( option_name )
  {}
  
  //! read value from option
  void read( void )
  { 
    std::string value( XmlOptions::get().raw( optionName() ) );
    int found( findText( value.c_str() ) );
    if( found < 0 ) 
    {
      addItem( value.c_str() );
      found = findText( value.c_str() );  
    }
    
    setCurrentIndex( found );
  }
  
  //! write value to option
  void write( void ) const
  { XmlOptions::get().setRaw( optionName(), qPrintable( currentText() ) ); }
        
};
#endif

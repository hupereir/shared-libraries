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

#ifndef OptionFontEdit_h
#define OptionFontEdit_h

/*!
   \file OptionFontEdit.h
   \brief QLineEdit associated to an option for configuration dialogs
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/
#include "FontEdit.h"
#include "Options.h"
#include "OptionWidget.h"

//! QLineEdit associated to an option for configuration dialogs
class OptionFontEdit: public FontEdit, public OptionWidget
{
  
  public:
  //! constructor
  OptionFontEdit( QWidget* parent, const std::string& option_name ):
      FontEdit( parent ),
      OptionWidget( option_name )
  {}
  
  //! read value from option
  void read( void )
  { setFont( XmlOptions::get().raw( optionName() ) ); }
  
  //! write value to option
  void write( void ) const
  { XmlOptions::get().setRaw( optionName(), qPrintable( font().toString() ) ); }
        
};

#endif


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

#ifndef OptionBrowsedLineEdit_h
#define OptionBrowsedLineEdit_h

/*!
   \file    OptionBrowsedLineEdit.h
   \brief   QLineEdit associated to an option for configuration dialogs
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "BrowsedLineEditor.h"
#include "ImprovedLineEditor.h"
#include "OptionWidget.h"
#include "XmlOptions.h"

//! QLineEdit associated to an option for configuration dialogs
class OptionBrowsedLineEditor: public BrowsedLineEditor, public OptionWidget
{
  
  public:
  
  //! constructor
  OptionBrowsedLineEditor( QWidget* parent, const std::string& option_name ):
      BrowsedLineEditor( parent ),
      OptionWidget( option_name )
  {}
  
  //! read value from option
  void read( void )
  { setFile( XmlOptions::get().raw( optionName() ) ); }
  
  //! write value to option
  void write( void ) const
  { XmlOptions::get().setRaw( optionName(), qPrintable( editor().text() ) ); }
        
};
#endif

#ifndef OptionWidgetList_h
#define OptionWidgetList_h

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
   \file    OptionWidgetList.h
   \brief   abstract container for OptionWidgets
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <vector>

#include "OptionWidget.h"

//! abstract container for OptionWidgets
/*! 
the class derive from OptionWidget too so that one can read/write all contained 
widget at once
*/
class OptionWidgetList: public OptionWidget
{
  
  public:
  
  //! constructor
  OptionWidgetList( void ):
    OptionWidget( "generic" )
  {}

  //! add option widget
  void addOptionWidget( OptionWidget* widget )
  { 
    Debug::Throw( "OptionWidgetList::addOptionWidget.\n" );
    option_widgets_.push_back( widget ); 
  }

  //! clear option widgets
  virtual void clearOptionWidgets( void )
  { 
    Debug::Throw( "OptionWidgetList::clearOptionWidgets.\n" );
    option_widgets_.clear(); 
  }
  
  //! read options
  virtual void read( void )
  {
    Debug::Throw( "OptionWidgetList::read.\n" );
    for( std::vector< OptionWidget* >::iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
    { (*iter)->read(); }
  }
  
  //! read options
  virtual void write( void ) const
  {
    Debug::Throw( "OptionWidgetList::write\n" );
    for( std::vector< OptionWidget* >::const_iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
    { (*iter)->write(); }
  }
  
  private:
  
  //! contained options
  std::vector< OptionWidget* > option_widgets_;
  
};

#endif


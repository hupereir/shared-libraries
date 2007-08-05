#ifndef HelpItemList_h
#define HelpItemList_h

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
   \file    HelpItemList.h
   \brief   list to display help item entries
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "CustomListBox.h"
#include "HelpItem.h"

namespace BASE
{
  //! Help item list
  class HelpItemList:public CustomListBox
  {
    
    public:
    //! constructor
    HelpItemList( QWidget *parent ):
      CustomListBox( parent )
      {}
      
    //! local item (overloaded) for counter dialog
    class Item: public QListWidgetItem
    {
        
      public:
      
      //! constructor
      Item( HelpItemList* parent, const HelpItem& item ):
        QListWidgetItem( parent ),
        item_( item )
      { setText(item.label().c_str() ); }
      
      //! return associated item
      const HelpItem& item( void ) const
      { return item_; }
      
      //! return associated item
      HelpItem& item( void )
      { return item_; }
      
      private:
      
      //! associated help text
      HelpItem item_;
      
    };  
    
  };
  
};

#endif

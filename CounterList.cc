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
  \file CounterList.cc
  \brief qlistview for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include "CounterList.h"

using namespace std;

//_______________________________________________
char* CounterList::column_titles_[ CounterList::n_columns ] = 
{
  "name",
  "count"
};

//_______________________________________________
CounterList::CounterList( QWidget* parent ):
  CustomListView( parent )
{ 
  Debug::Throw( "CounterList::CounterList.\n" ); 

  setRootIsDecorated( false );
  setSortingEnabled( true );
  
  // number of columns
  setColumnCount( n_columns );
  
  // set column names
  for( unsigned int i=0; i<n_columns; i++ ) 
  { setColumnName( i, column_titles_[i] ); }

};        

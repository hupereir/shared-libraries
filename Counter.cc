// $Id$

/******************************************************************************
*                         
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>             
*                         
* This is free software; you can redistribute it and/or modify it under the    
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
   \file    Counter.cc
   \brief   Object counter
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Counter.h"

using namespace std;

//____________________________________________________
Counter::CounterMap Counter::counts_;

//____________________________________________________
void Counter::print( ostream& out )
{
  for( CounterMap::iterator iter = counts_.begin(); iter != counts_.end(); iter++ )
  out << iter->first << ": " << iter->second << endl;
  out << endl;
}


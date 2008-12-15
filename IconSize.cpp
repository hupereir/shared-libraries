
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
  \file IconSize.cpp
  \brief default icon sizes
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QStyle>

#include "IconSize.h"

using namespace std;

//__________________________________________________________________
IconSize::IconSize( IconSize::Size size )
{

  assert( size != DEFAULT );
  setWidth( size );
  setHeight( size );
  
}

//__________________________________________________________________
IconSize::IconSize( QWidget* parent, IconSize::Size size )
{
  
  int icon_size( size );
  if( size == DEFAULT )
  { 
    assert( parent );
    icon_size =  parent->style()->pixelMetric( QStyle::PM_ToolBarIconSize );
  }
  
  setWidth( icon_size );
  setHeight( icon_size );
  
}

//______________________________________________________________________
IconSize::Map& IconSize::map( void )
{ 
  static Map size_map;
  if( size_map.empty() )
  {
    size_map.insert( make_pair( DEFAULT, "&Default" ) );
    size_map.insert( make_pair( SMALL, "&Small (16x16)" ) );
    size_map.insert( make_pair( MEDIUM, "&Medium (22x22)" ) );
    size_map.insert( make_pair( LARGE, "&Large (32x32)") );
    size_map.insert( make_pair( HUGE, "&Huge (48x48)" ) );
  }
  
  return size_map;
}

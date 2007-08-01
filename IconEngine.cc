 
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
  \file IconEngine.cc
  \brief customized Icon factory to provide better looking disabled icons
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomPixmap.h"
#include "IconEngine.h"

using namespace std;

//__________________________________________________________
QIcon IconEngine::get( const QPixmap& pixmap )
{
  
  Debug::Throw( "IconEngine::get (QPixmap).\n" );
  if( pixmap.isNull() ) return QIcon( pixmap );
  
  QIcon out( pixmap );
  out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::On );
  out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::Off );
  return out;
  
}


//__________________________________________________________
QIcon IconEngine::get( const QIcon& icon )
{
  
  Debug::Throw( "IconEngine::get (QIcon).\n" );
  
  QIcon out( icon );
  QPixmap pixmap;
  if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::On )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::On );
  if( !(pixmap = icon.pixmap( QIcon::Normal, QIcon::Off )).isNull() ) out.addPixmap( CustomPixmap( pixmap ).disabled(), QIcon::Disabled, QIcon::Off );
  return out;
  
}

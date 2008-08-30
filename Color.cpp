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
  \file Color.h
  \brief color utilities
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "Color.h"


//__________________________________________________________
QColor BASE::Color::merge( QColor other, double intensity )
{
  if( !isValid() ) return other;
  if( !other.isValid() ) return *this;
  if( other == *this ) return *this;

  double red = intensity*Color::red() + (1.0-intensity )*other.red();
  double green = intensity*Color::green() + (1.0-intensity )*other.green();
  double blue = intensity*Color::blue() + (1.0-intensity )*other.blue();
  double alpha = intensity*Color::alpha() + (1.0-intensity )*other.alpha();

  return QColor( int( red ), int( green ), int( blue ), int( alpha ) );
}

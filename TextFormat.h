// $Id$
#ifndef _TextFormatInfo_h_
#define _TextFormatInfo_h_
 
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
  \file TextFormatInfo.h
  \brief tracks text format in given text range
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <iostream>
#include <list>

namespace FORMAT
{
  //! format bit enumaration
  enum 
  {
    //! default
    DEFAULT = 0,
    
    //! bold bit
    BOLD = 1<<0,
        
    //! italic bit
    ITALIC = 1<<1,
        
    //! underline bit
    UNDERLINE = 1<<2,
        
    //! strike bit
    STRIKE = 1<<3,
        
    //! underline bit
    OVERLINE = 1<<4
  
  };
  
};

#endif

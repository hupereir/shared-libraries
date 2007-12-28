#ifndef SystemEnvironmentDialog_h
#define SystemEnvironmentDialog_h

// $Id$

/******************************************************************************
 *                            
 * Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>         G       
 *                            
 * This is free software; you can redistribute it and/or modify it under the      
 * terms of the GNU General Public License as published by the Free Software      
 * Foundation; either version 2 of the License, or (at your option) any later    
 * version.                                    
 *                             
 * This software is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or            
 * FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License            
 * for more details.                        
 *                             
 * You should have received a copy of the GNU General Public License along with 
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple       
 * Place, Suite 330, Boston, MA   02111-1307 USA                                      
 *                            
 *                            
 *******************************************************************************/

/*!
   \file SystemEnvironmentDialog.h
   \brief displays environment variables
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <map>
#include <string>
#include <QDialog>

#include "Counter.h"

//! displays Counter names and counts
class SystemEnvironmentDialog: public QDialog, public Counter
{
  
  public:
          
  //! constructor
  SystemEnvironmentDialog( QWidget* parent );
  
  //! destructor
  virtual ~SystemEnvironmentDialog( void )
  {}
  
};

#endif

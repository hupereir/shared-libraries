#ifndef CounterDialog_h
#define CounterDialog_h

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
   \file CounterDialog.h
   \brief displays Counter names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <map>
#include <string>

#include "CounterList.h"
#include "CustomDialog.h"

//! displays Counter names and counts
class CounterDialog: public CustomDialog
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
          
  // constructor
  CounterDialog( QWidget* parent );
  
  //! destructor
  virtual ~CounterDialog( void )
  {}

  public slots:
  
  //! update Counter list
  void Update();
  
  private:
  
  //! Counter list
  CounterList* counter_list_;
 
  //! map to store Counter list items agains names
  std::map< std::string, QTreeWidgetItem* > items_;  
  
};

#endif

#ifndef _ListViewConfiguration_h_
#define _ListViewConfiguration_h_

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
  \file ListViewConfiguration.h
  \brief Configuration vbox for ListView columns
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QCheckBox>
#include <QTreeWidget>

#include <string>
#include <vector>

#include "Counter.h"

//! Configuration vbox for ListView columns
class ListViewConfiguration: public QWidget, public Counter
{
  

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! constructor
  ListViewConfiguration( QWidget* parent, QTreeWidget* target, const std::string& title = ""  );
  
  public slots:
  
  //! restore configuration from initial point
  void restore( void );
      
  //! update target listbox according to the checkboxes status
  void update( void );
  
  private:

  //! compute check button state mask to keep track of modifications
  unsigned int _computeMask( void ) const;
           
  //! returns true if button has been modified wrt its initial state
  bool _modified( const unsigned int& index ) const;
      
  //! target ListView
  QTreeWidget *target_;
        
  //! checkboxes  
  std::vector< QCheckBox* > checkbox_;
  
  //! mask (set at constructor)
  unsigned int modified_mask_;
   
  //! mask (set at constructor)
  unsigned int backup_mask_;
     
};
#endif

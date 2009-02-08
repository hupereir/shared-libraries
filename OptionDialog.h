#ifndef OptionDialog_h
#define OptionDialog_h

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
   \file OptionDialog.h
   \brief displays all run-time options
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <map>


#include "BaseDialog.h"
#include "OptionModel.h"
#include "XmlOptions.h"

// forward declaration
class TreeView;

//! displays Counter names and counts
class OptionDialog: public BaseDialog
{

  Q_OBJECT
  
  public:
          
  //! constructor
  OptionDialog( QWidget* parent );
  
  //! destructor
  virtual ~OptionDialog( void )
  {}
  
  signals:
  
  //! emmited when options are modified
  void configurationChanged( void );

  protected slots:
  
  //! reload options from backup
  void _reload( void );
  
  //! option modified
  void _optionModified( OptionPair );

  //! option modified
  void _specialOptionModified( OptionPair );
  
  private:
  
  //! list
  TreeView& _list( void ) const
  { 
    assert( list_ );
    return *list_;
  }
  
  //! option model
  OptionModel model_;

  //! list
  TreeView* list_;
  
  //! pointer to original options set
  /*!
    it is needed to keep track of the changes
    so that initial set is restored when pressing the cancel button
  */
  Options backup_options_;

};

#endif

#ifndef NewItemDialog_h
#define NewItemDialog_h

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
   \file NewItemDialog.h
   \brief Edit/create keyword popup dialog
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QString>

#include "CustomDialog.h"
#include "LineEditor.h"

//! Edit/create keyword popup dialog
class NewItemDialog: public CustomDialog
{
  
  public:
      
  //! constructor
  NewItemDialog( QWidget* parent );
  
  //! destructor
  virtual ~NewItemDialog( void )
  {}
 
  //! keyword
  void setItemName( const QString& name )
  { editor_->setText( name ); }
  
  //! keyword
  QString itemName( void ) const
  { return editor_->text(); }
        
  private:
        
  //! editor
  LineEditor* editor_;
  
};

#endif

#ifndef PixmapCacheDialog_h
#define PixmapCacheDialog_h

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
   \file PixmapCacheDialog.h
   \brief displays PixmapCache names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <map>


#include "BaseDialog.h"
#include "Counter.h"
#include "PixmapCacheModel.h"

class TreeView;

//! displays PixmapCache names and counts
class PixmapCacheDialog: public BaseDialog, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
          
  // constructor
  PixmapCacheDialog( QWidget* parent );
  
  //! destructor
  virtual ~PixmapCacheDialog( void )
  {}

  public slots:
  
  //! update PixmapCache list
  void update();
  
  private:
  
  //! model
  PixmapCacheModel model_;

  //! list
  TreeView* list_;
  
};

#endif

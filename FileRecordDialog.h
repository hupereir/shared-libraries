#ifndef FileRecordDialog_h
#define FileRecordDialog_h

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
   \file FileRecordDialog.h
   \brief displays Counter names and counts
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <assert.h>
#include <map>
#include <string>

#include <QDialog>

#include "FileRecordModel.h"

class FileList;
class TreeView;

//! displays Counter names and counts
class FileRecordDialog: public QDialog, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
          
  // constructor
  FileRecordDialog( QWidget* parent, FileList&  );
  
  //! destructor
  virtual ~FileRecordDialog( void )
  {}

  public slots:
  
  //! update Counter list
  void update();
  
  private:
  
  //! file list
  FileList& _fileList( void ) const
  {
    assert( file_list_ );
    return *file_list_;
  }
  
  //! tree view
  TreeView& _list( void ) const
  { 
    assert( list_ );
    return *list_; 
  }
  
  //! file list
  FileList* file_list_;
  
  //! model
  FileRecordModel model_;

  //! list
  TreeView* list_;
  
};

#endif

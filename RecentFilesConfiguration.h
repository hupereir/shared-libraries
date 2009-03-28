
#ifndef _RecentFilesConfiguration_h_
#define _RecentFilesConfiguration_h_

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
   \file    RecentFilesConfiguration.h
   \brief   configure files in recent files menu
   \version $Revision$
   \date    $Date$
*/

#include <QAction>
#include <QPushButton>
#include <QWidget>

#include "Counter.h"
#include "FileRecordModel.h"
#include "OptionWidgetList.h"

class FileList;
class TreeView;

//! configure files in recent files menu
class RecentFilesConfiguration: public QWidget, public OptionWidgetList, public Counter
{
  
  Q_OBJECT
  
  public:
      
  //! constructor
  RecentFilesConfiguration( QWidget*, FileList& );
  
  //! read options
  virtual void read( void );
  
  //! read options
  virtual void write( void ) const;
 
  private slots:
  
  //! update actions
  void _updateButtons( void );
  
  //! clean
  void _clean( void );
  
  //! remove 
  void _remove( void );
  
  //! reload
  void _reload( void );
  
  protected:
  
  //! file list
  FileList& _recentFiles( void ) const
  { return *recent_files_; }
  
  //! model
  FileRecordModel& _model( void )
  { return model_; }
   
  //! model
  const FileRecordModel& _model( void ) const
  { return model_; }
 
  //! list
  TreeView& _list( void ) const
  { return *list_; }

  //! clean
  QPushButton& _cleanButton( void ) const
  { return *clean_button_; }

  //! remove
  QPushButton& _removeButton( void ) const
  { return *remove_button_; }
  
  //! reload
  QPushButton& _reloadButton( void ) const
  { return *reload_button_; }
  
  //! clean
  QAction& _cleanAction( void ) const
  { return *clean_action_; }

  //! remove
  QAction& _removeAction( void ) const
  { return *remove_action_; }
  
  //! reload
  QAction& _reloadAction( void ) const
  { return *reload_action_; }

  private:

  //! file list
  FileList* recent_files_;
  
  //! file record model
  FileRecordModel model_;
  
  //! file record list
  TreeView* list_;
 
  //! clean
  QPushButton *clean_button_;

  //! remove
  QPushButton *remove_button_;
 
  //! reload
  QPushButton *reload_button_;
  
  //!@name actions
  //@{
  
  //! clean action
  QAction* clean_action_;
  
  //! remove action
  QAction* remove_action_;

  //! remove action
  QAction* reload_action_;
  
  //@}
  
};

#endif

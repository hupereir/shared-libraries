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
  \file DebugMenu.h
  \brief some basic runtime debuging
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _DebugMenu_h_
#define _DebugMenu_h_

#include <QMenu>
#include <QDialog>

#include "Counter.h"

class CounterDialog;
class IconCacheDialog;
class FileRecordDialog;

/*!
  \class DebugMenu
  \brief some basic runtime debuging
*/

class DebugMenu: public QMenu, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public: 
  
  //! constructor
  DebugMenu( QWidget* parent );
  
  //! destructor
  ~DebugMenu( void ) 
  {}
  
  private slots:

  //! print object counters
  void _showCounterDialog( void ); 
 
  //! show system environment
  void _showRecentFiles( void );
 
  //! icon cache dialog
  void _showIconCacheDialog( void );
  
  //! show system environment
  void _showSystemEnvironment( void );
  
  //! show options
  void _showOptions( void );
  
  private: 

  //! non modal Counter dialog
  CounterDialog* counter_dialog_;
  
  //! non modal icon cache dialog
  IconCacheDialog* icon_cache_dialog_;

  //! file records
  FileRecordDialog* recent_files_dialog_;
     
};
#endif

#ifndef _ColumnSortingMenu_h_
#define _ColumnSortingMenu_h_

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
  \file ColumnSortingMenu.h
  \brief handels column visibility in TreeViews
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QTreeView>
#include <map>

#include "Counter.h"
#include "Debug.h"

//_______________________________________________________________
class ColumnSortingMenu:public QMenu, public Counter
{
  
  //! Qt meta object declaration
  Q_OBJECT
    
  public:
  
  //! constructor
  ColumnSortingMenu( QWidget*, QTreeView*, const QString& title = "Sort &By" );

  private slots:

  //! update actions
  void _updateActions( void );
  
  //! update mask when triggering actions
  void _sort( QAction* action );
 
  private:
   
  //! target
  QTreeView& _target( void )
  { 
    assert( target_ );
    return *target_;
  }
  
  //! target
  QTreeView* target_;
    
  //! action group
  QActionGroup* group_;
  
  //! map action to column index
  typedef std::map< QAction*, int > ActionMap;
  
  //! map action to column index
  ActionMap actions_;
        
};

#endif

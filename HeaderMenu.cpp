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
  \file HeaderMenu.h
  \brief handels column visibility in TreeViews
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QActionGroup>
#include <QHeaderView>
#include <assert.h>

#include "Debug.h"
#include "HeaderMenu.h"

using namespace std;

//_________________________________________________________________
HeaderMenu::HeaderMenu( QTreeView* parent ):
  QMenu( parent ),
  Counter( "HeaderMenu" )
{
  
  Debug::Throw( "HeaderMenu::HeaderMenu.\n" );
  QHeaderView* header( parent->header() );
  assert( header );
    
  // loop over columns in header
  unsigned int visible_columns(0);
  for( int index=0; index < header->count(); index++ )
  {
    
    // retrieve column name
    QString column_name( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );    
    if( column_name.isNull() || column_name.isEmpty() )
    {
      ostringstream what;
      what << "column " << index+1;
      column_name = what.str().c_str();
    }    
  
    QAction* action = new QAction( column_name, this );
    action->setCheckable( true );
    action->setChecked( !parent->isColumnHidden( index ) );
    if( !parent->isColumnHidden( index ) ) visible_columns++;
    
    addAction( action );
    actions_.insert( make_pair( action, index ) );

  }
  
  // connections
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _updateList( QAction* ) ) );
  
  // if only one column is visible, disable corresponding action
  if( visible_columns == 1 )
  {
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); iter++ )
    { if( iter->first->isChecked() ) iter->first->setEnabled( false ); }
  }
  
}

//______________________________________________________________________________
void HeaderMenu::_updateList( QAction* action )
{
  Debug::Throw( "HeaderMenu::_updateList" );
  
  // retrieve index
  ActionMap::const_iterator iter = actions_.find( action );
  assert( iter != actions_.end() );
  
  // retrieve parent tree_view
  QTreeView* tree_view = dynamic_cast<QTreeView*>( parentWidget() );
  assert( tree_view );  

  // set column visibility
  tree_view->setColumnHidden( iter->second, !iter->first->isChecked() );
  
}

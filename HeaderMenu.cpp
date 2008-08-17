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

//_____________________________________________________
void HeaderMenu::installSelectionActions( QTreeView* parent )
{

  Debug::Throw( "HeaderMenu::installSelectionActions.\n" );
  
  // retrieve parent header.
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
    selection_actions_.insert( make_pair( action, index ) );

  }
  
  // connections
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _updateSelectedColumns( QAction* ) ) );
  
  // if only one column is visible, disable corresponding action
  if( visible_columns == 1 )
  {
    for( ActionMap::iterator iter = selection_actions_.begin(); iter != selection_actions_.end(); iter++ )
    { if( iter->first->isChecked() ) iter->first->setEnabled( false ); }
  }

}

//_____________________________________________________
void HeaderMenu::installSortActions( QTreeView* parent )
{

  Debug::Throw( "HeaderMenu::installSortActions.\n" );
    
  // retrieve parent header.
  QHeaderView* header( parent->header() );
  assert( header );
  assert( header->isSortIndicatorShown() );
  
  // create action group
  QActionGroup* group( new QActionGroup( this ) );
  group->setExclusive( true );
  
  // at seperator and submenu
  addSeparator();
  QMenu* menu = addMenu( "Sort &By" );
  
  // loop over columns in header
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
  
    QAction* action = new QAction( column_name, menu );
    action->setCheckable( true );
    action->setChecked( index == header->sortIndicatorSection() );
    
    menu->addAction( action );
    group->addAction( action );
    sort_actions_.insert( make_pair( action, index ) );

  }

  // connections
  connect( menu, SIGNAL( triggered( QAction* ) ), SLOT( _sort( QAction* ) ) );
  
}

//______________________________________________________________________________
void HeaderMenu::_updateSelectedColumns( QAction* action )
{
  Debug::Throw( "HeaderMenu::_updateSelectedColumns.\n" );
  
  // retrieve index
  ActionMap::const_iterator iter = selection_actions_.find( action );
  if( iter == selection_actions_.end() ) return;
  
  // retrieve parent tree_view
  QTreeView* tree_view = dynamic_cast<QTreeView*>( parentWidget() );
  assert( tree_view );  

  // set column visibility
  tree_view->setColumnHidden( iter->second, !iter->first->isChecked() );
  
}


//______________________________________________________________________________
void HeaderMenu::_sort( QAction* action )
{
  Debug::Throw( "HeaderMenu::_sort.\n" );
  
  // retrieve index
  ActionMap::const_iterator iter = sort_actions_.find( action );
  if( iter == sort_actions_.end() ) return;
  
  // retrieve parent tree_view
  QTreeView* tree_view = dynamic_cast<QTreeView*>( parentWidget() );
  assert( tree_view );  
  QHeaderView* header = tree_view->header();
  assert( header );

  header->setSortIndicator( iter->second, header->sortIndicatorOrder() );
  
}


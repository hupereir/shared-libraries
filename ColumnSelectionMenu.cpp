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

#include "ColumnSelectionMenu.h"

#include "Debug.h"
#include "TreeView.h"

#include <QHeaderView>

//_____________________________________________________
ColumnSelectionMenu::ColumnSelectionMenu( QWidget* parent, QTreeView* target, const QString& title ):
    QMenu( parent ),
    Counter( "ColumnSelectionMenu" ),
    target_( target )
{
    Debug::Throw( "ColumnSelectionMenu::ColumnSelectionMenu.\n" );
    setTitle( title );
    connect( this, SIGNAL( aboutToShow( void ) ), SLOT( _updateActions( void ) ) );
    connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _updateSelectedColumns( QAction* ) ) );
}

//_____________________________________________________
void ColumnSelectionMenu::_updateActions( void )
{

    Debug::Throw( "ColumnSelectionMenu::_updateActions.\n" );

    // clear existing actions
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }
    actions_.clear();

    // check if the menu already has actions.
    QList<QAction*> actions( ColumnSelectionMenu::actions() );
    QAction *firstAction( actions.isEmpty() ? 0:actions.front() );

    // retrieve parent header.
    QHeaderView* header( target_->header() );
    Q_ASSERT( header );

    // try cast to treeview
    TreeView* treeView( qobject_cast<TreeView*>( target_ ) );

    // loop over columns in header
    unsigned int visibleColumns(0);
    for( int index=0; index < header->count(); index++ )
    {

        // retrieve column name
        QString columnName( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() ) columnName = QString( tr("column %1") ).arg( index+1 );

        QAction* action = new QAction( columnName, this );
        action->setCheckable( true );
        action->setChecked( !target_->isColumnHidden( index ) );

        // disable/hide action for locked columns
        if( treeView && treeView->isColumnVisibilityLocked( index ) )
        {
            if( action->isChecked() ) action->setEnabled( false );
            else action->setVisible( false );
        }

        if( !target_->isColumnHidden( index ) ) visibleColumns++;

        insertAction( firstAction, action );
        actions_.insert( action, index );

    }

    // if only one column is visible, disable corresponding action
    if( visibleColumns == 1 )
    {
        for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
        { if( iter.key()->isChecked() ) iter.key()->setEnabled( false ); }
    }

}

//______________________________________________________________________________
void ColumnSelectionMenu::_updateSelectedColumns( QAction* action )
{
    Debug::Throw( "ColumnSelectionMenu::_updateSelectedColumns.\n" );

    // retrieve index
    ActionMap::const_iterator iter = actions_.find( action );
    if( iter == actions_.end() ) return;

    // set column visibility
    target_->setColumnHidden( iter.value(), !iter.key()->isChecked() );

    if( TreeView* treeView = qobject_cast<TreeView*>( target_ ) )
    { treeView->saveMask(); }

}

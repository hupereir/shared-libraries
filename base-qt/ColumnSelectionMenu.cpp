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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
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
    connect( this, SIGNAL(aboutToShow()), SLOT(_updateActions()) );
    connect( this, SIGNAL(triggered(QAction*)), SLOT(_updateSelectedColumns(QAction*)) );
}

//_____________________________________________________
void ColumnSelectionMenu::_updateActions()
{

    Debug::Throw( "ColumnSelectionMenu::_updateActions.\n" );

    // clear current column action
    currentColumnAction_.reset();

    // clear existing actions
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }

    actions_.clear();

    // check if the menu already has actions.
    QList<QAction*> actions( ColumnSelectionMenu::actions() );
    QAction *firstAction( actions.isEmpty() ? nullptr:actions.front() );

    // retrieve parent header.
    QHeaderView* header( target_->header() );
    Q_ASSERT( header );

    // try cast to treeview
    TreeView* treeView( qobject_cast<TreeView*>( target_ ) );

    if( currentColumn_ >= 0 )
    {

        // retrieve column name
        QString columnName( header->model()->headerData( currentColumn_, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() ) columnName = tr( "Column %1" ).arg( currentColumn_+1 );

        // disable/hide action for locked columns
        if( !( treeView && treeView->isColumnVisibilityLocked( currentColumn_ ) ) )
        {
            currentColumnAction_.reset( new QAction( tr( "Hide Column '%1'" ).arg( columnName ), this ) );
            connect( currentColumnAction_.get(), SIGNAL(triggered()), SLOT(_hideCurrentColumn()) );
            insertAction( firstAction, currentColumnAction_.get() );
            insertSeparator( firstAction );
        }

    }

    // loop over columns in header
    int visibleColumns(0);
    for( int index=0; index < header->count(); index++ )
    {

        // retrieve column name
        QString columnName( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() ) columnName = tr( "Column %1" ).arg( index+1 );

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
        for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
        { if( iter.key()->isChecked() ) iter.key()->setEnabled( false ); }
    }

}

//______________________________________________________________________________
void ColumnSelectionMenu::_hideCurrentColumn()
{
    Debug::Throw( "ColumnSelectionMenu::_hideCurrentColumn.\n" );

    // update current column visibility
    if( currentColumn_ >= 0 )
    { target_->setColumnHidden( currentColumn_, true ); }

    // save
    if( TreeView* treeView = qobject_cast<TreeView*>( target_ ) )
    { treeView->saveMask(); }

}

//______________________________________________________________________________
void ColumnSelectionMenu::_updateSelectedColumns( QAction* action )
{
    Debug::Throw( "ColumnSelectionMenu::_updateSelectedColumns.\n" );

    // retrieve index
    auto&& iter = actions_.find( action );
    if( iter == actions_.end() ) return;

    // set column visibility
    target_->setColumnHidden( iter.value(), !iter.key()->isChecked() );

    if( auto treeView = qobject_cast<TreeView*>( target_ ) )
    { treeView->saveMask(); }

}

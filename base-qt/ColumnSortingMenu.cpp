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

#include "ColumnSortingMenu.h"
#include "Debug.h"
#include "TreeView.h"

#include <QHeaderView>

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, const QString& title ):
    QMenu( parent ),
    Counter( "ColumnSortingMenu" ),
    group_( new QActionGroup( this ) )
{
    Debug::Throw( "ColumnSortingMenu::ColumnSortingMenu.\n" );
    setTitle( title );
    connect( this, SIGNAL(aboutToShow()), SLOT(_updateActions()) );
    connect( this, SIGNAL(triggered(QAction*)), SLOT(_sort(QAction*)) );
    group_->setExclusive( true );

    addSeparator();

    addAction( tr( "Reverse Order" ), this, SLOT(_revertOrder()) );
}

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, QTreeView* target, const QString& title ):
    ColumnSortingMenu( parent, title )
{ target_ = target; }

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, QHeaderView* header, const QString& title ):
    ColumnSortingMenu( parent, title )
{ header_ = header; }

//_____________________________________________________
void ColumnSortingMenu::_updateActions()
{

    Debug::Throw( "ColumnSortingMenu::_updateActions.\n" );

    // clear existing actions
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }
    actions_.clear();

    // check if the menu already has actions.
    QList<QAction*> actions( ColumnSortingMenu::actions() );
    QAction *firstAction( actions.isEmpty() ? 0:actions.front() );

    // retrieve parent header.
    if( target_ ) header_ = target_->header();

    // try cast to treeview
    TreeView* treeView( qobject_cast<TreeView*>( header_->parentWidget() ) );

    // loop over columns in header
    for( int index=0; index < header_->count(); index++ )
    {

        // retrieve column name
        QString columnName( header_->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() )
        { columnName = tr( "Column %1" ).arg( index+1 ); }

        auto action = new QAction( columnName, this );
        action->setCheckable( true );
        action->setChecked( index == header_->sortIndicatorSection() );

        // disable/hide action for locked columns
        if( treeView && treeView->isColumnVisibilityLocked( index ) )
        {
            if( treeView->isColumnHidden( index ) && !action->isChecked() )
            { action->setVisible( false ); }
        }

        insertAction( firstAction, action );
        group_->addAction( action );
        actions_.insert( action, index );

    }

}

//______________________________________________________________________________
void ColumnSortingMenu::_sort( QAction* action )
{
    Debug::Throw( "ColumnSortingMenu::_sort.\n" );

    // retrieve index
    auto iter = actions_.find( action );
    if( iter == actions_.end() ) return;

    // retrieve parent tree_view
    if( target_ ) header_ = target_->header();
    Q_ASSERT( header_ );

    header_->setSortIndicator( iter.value(), header_->sortIndicatorOrder() );

}

//______________________________________________________________________________
void ColumnSortingMenu::_revertOrder()
{
    Debug::Throw( "ColumnSortingMenu::_sort.\n" );

    // retrieve parent tree_view
    if( target_ ) header_ = target_->header();
    Q_ASSERT( header_ );

    Qt::SortOrder order( header_->sortIndicatorOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder );
    header_->setSortIndicator( header_->sortIndicatorSection(), order );

}

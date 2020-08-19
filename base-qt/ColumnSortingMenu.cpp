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
#include "ReverseOrderAction.h"
#include "TreeView.h"

#include <QHeaderView>

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, const QString& title ):
    QMenu( parent ),
    Counter( QStringLiteral("ColumnSortingMenu") ),
    group_( new QActionGroup( this ) )
{
    Debug::Throw( QStringLiteral("ColumnSortingMenu::ColumnSortingMenu.\n") );
    setTitle( title );
    connect( this, &QMenu::aboutToShow, this, &ColumnSortingMenu::_updateActions );
    connect( this, &QMenu::triggered, this, &ColumnSortingMenu::_sort );
    group_->setExclusive( true );
}

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, QTreeView* target, const QString& title ):
    ColumnSortingMenu( parent, title )
{ 
    target_ = target; 
    addSeparator();
    addAction( new ReverseOrderAction( this, target_ ) );
}

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, QHeaderView* header, const QString& title ):
    ColumnSortingMenu( parent, title )
{ 
    header_ = header; 
    addSeparator();
    addAction( new ReverseOrderAction( this, header_ ) );
}

//_____________________________________________________
void ColumnSortingMenu::_updateActions()
{

    Debug::Throw( QStringLiteral("ColumnSortingMenu::_updateActions.\n") );

    // clear existing actions
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }
    actions_.clear();

    // check if the menu already has actions.
    auto actions( this->actions() );
    auto firstAction( actions.isEmpty() ? nullptr:actions.front() );

    // retrieve parent header.
    if( target_ ) header_ = target_->header();

    // try cast to treeview
    auto treeView = qobject_cast<TreeView*>( header_->parentWidget() );

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
    Debug::Throw( QStringLiteral("ColumnSortingMenu::_sort.\n") );

    // retrieve index
    auto iter = actions_.find( action );
    if( iter == actions_.end() ) return;

    // retrieve parent tree_view
    if( target_ ) header_ = target_->header();
    if( header_ ) header_->setSortIndicator( iter.value(), header_->sortIndicatorOrder() );
}

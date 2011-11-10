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


#include <QHeaderView>

#include "Debug.h"
#include "ColumnSortingMenu.h"

//_____________________________________________________
ColumnSortingMenu::ColumnSortingMenu( QWidget* parent, QTreeView* target, const QString& title ):
QMenu( parent ),
Counter( "ColumnSortingMenu" ),
target_( target ),
group_( new QActionGroup( this ) )
{
    Debug::Throw( "ColumnSortingMenu::ColumnSortingMenu.\n" );
    setTitle( title );
    connect( this, SIGNAL( aboutToShow( void ) ), SLOT( _updateActions( void ) ) );
    connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _sort( QAction* ) ) );
    group_->setExclusive( true );

    addSeparator();

    addAction( "&Reverse order", this, SLOT( _revertOrder( void ) ) );
}

//_____________________________________________________
void ColumnSortingMenu::_updateActions( void )
{

    Debug::Throw( "ColumnSortingMenu::_updateActions.\n" );

    // clear existing actions
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter->first; }
    actions_.clear();

    // check if the menu already has actions.
    QList<QAction*> actions( ColumnSortingMenu::actions() );
    QAction *first_action( actions.isEmpty() ? 0:actions.front() );

    // retrieve parent header.
    QHeaderView* header( _target().header() );
    assert( header );
    assert( header->isSortIndicatorShown() );

    // loop over columns in header
    for( int index=0; index < header->count(); index++ )
    {

        // retrieve column name
        QString column_name( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( column_name.isNull() || column_name.isEmpty() )
        {
            QString buffer;
            QTextStream( &buffer ) << "column " << index+1;
            column_name = buffer;
        }

        QAction* action = new QAction( column_name, this );
        action->setCheckable( true );
        action->setChecked( index == header->sortIndicatorSection() );

        insertAction( first_action, action );
        group_->addAction( action );
        actions_.insert( std::make_pair( action, index ) );

    }

}

//______________________________________________________________________________
void ColumnSortingMenu::_sort( QAction* action )
{
    Debug::Throw( "ColumnSortingMenu::_sort.\n" );

    // retrieve index
    ActionMap::const_iterator iter = actions_.find( action );
    if( iter == actions_.end() ) return;

    // retrieve parent tree_view
    QHeaderView* header = _target().header();
    assert( header );

    header->setSortIndicator( iter->second, header->sortIndicatorOrder() );

}

//______________________________________________________________________________
void ColumnSortingMenu::_revertOrder( void )
{
    Debug::Throw( "ColumnSortingMenu::_sort.\n" );

    // retrieve parent tree_view
    QHeaderView* header = _target().header();
    assert( header );

    Qt::SortOrder order( header->sortIndicatorOrder() == Qt::AscendingOrder ? Qt::DescendingOrder:Qt::AscendingOrder );
    header->setSortIndicator( header->sortIndicatorSection(), order );

}

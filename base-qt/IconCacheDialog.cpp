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

#include "IconCacheDialog.h"

#include "BaseIconNames.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "TreeView.h"

#include <QPushButton>
#include <QLayout>

//__________________________________________________________________________
IconCacheDialog::IconCacheDialog( QWidget* parent ):
    CustomDialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( "IconCacheDialog::IconCacheDialog.\n" );
    setWindowTitle( "Icon Cache" );
    setOptionName( "ICON_CACHE_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    // insert main vertical box
    list_ = new TreeView( this );
    mainLayout().addWidget( list_ );
    list_->setModel( &model_ );
    list_->setIconSize( IconSize( IconSize::Large ) );

    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( IconNames::Reload ), tr( "Update" ), this ) );
    connect( button, SIGNAL(clicked()), SLOT(update()) );
    button->setAutoDefault( false );

}

//__________________________________________________________________________
void IconCacheDialog::update( void )
{

    Debug::Throw( "IconCacheDialog::update.\n" );

    // retrieve cache
    const Base::IconCache& cache( IconEngine::cache() );
    IconCacheModel::List modelList;
    for( Base::IconCache::const_iterator iter = cache.begin(); iter != cache.end(); ++iter )
    { modelList << Base::IconCache::Pair( iter.key(), iter.value() ); }

    model_.set( modelList );

    list_->resizeColumnToContents( IconCacheModel::Icon );

}

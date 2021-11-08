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

#include "BaseIconNames.h"
#include "IconCacheDialog.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "QtUtil.h"
#include "TreeView.h"


#include <QPushButton>
#include <QLayout>

//__________________________________________________________________________
IconCacheDialog::IconCacheDialog( QWidget* parent ):
    Dialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( QStringLiteral("IconCacheDialog::IconCacheDialog.\n") );
    setWindowTitle( tr( "Icon Cache" ) );
    setOptionName( QStringLiteral("ICON_CACHE_DIALOG") );

    QtUtil::setMargin(layout(), 0);
    QtUtil::setMargin(&buttonLayout(), 5);

    // insert main vertical box
    list_ = new TreeView( this );
    mainLayout().addWidget( list_ );
    list_->setModel( &model_ );
    list_->setIconSize( IconSize::get( IconSize::Large ) );

    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( IconNames::Reload ), tr( "Update" ), this ) );
    connect( button, &QPushButton::clicked, this, [this](bool){ updateCache(); } );
    button->setAutoDefault( false );

}

//__________________________________________________________________________
void IconCacheDialog::updateCache()
{

    Debug::Throw( QStringLiteral("IconCacheDialog::updateCache.\n") );

    // retrieve cache
    const Base::IconCache& cache( IconEngine::cache() );
    IconCacheModel::List modelList;
    for( auto&& iter = cache.begin(); iter != cache.end(); ++iter )
    { modelList.append( Base::IconPair( iter.key(), iter.value() ) ); }

    model_.set( modelList );

    list_->resizeColumnToContents( IconCacheModel::Icon );

}
